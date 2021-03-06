#include <stdio.h>
#include <lwip/tcp.h>
#include <lwip/tcpip.h>
#include <arch/sys_arch.h>

#define FATAL(fmt,...) do { printf("FATAL: " fmt "\n",## __VA_ARGS__); } while(0)



static void set_open_thread_gate_callback(void* arg)
{
	printf("[TCP] Opening gate for Main...\n");
	fflush(stdout);
	sys_thread_gate_open((sys_thread_gate_t*)arg);
	printf("[TCP] Gate opened for Main\n");
	fflush(stdout);
}

// gate A closed gate that will be opened when the component is initialized.
void tcpip_init_block(sys_thread_gate_t* gate)
{
	//tcpip_init(sys_thread_gate_open, gate);
	tcpip_init(set_open_thread_gate_callback, gate);

	printf("[Main] Waiting for tcpip_init to open gate...\n");
	sys_thread_gate_wait(gate);
	printf("[Main] Done waiting for tcpip_init\n");
}


err_t accept_handler(void* arg, struct tcp_pcb *newpcb, err_t err)
{
	printf("accept_handler(arg=%p, newpcb=%p, err=%d)\n", arg, newpcb, err);
	return ERR_UNKNOWN;
}


//
// Default network Interface
//
struct netif default_interface;
err_t netif_input(struct pbuf* p, struct netif* inp)
{
	printf("netif_input\n");
	return ERR_OK;
}
err_t netif_output(struct netif* netif, struct pbuf* p, const ip4_addr_t* ipaddr)
{
	printf("netif_output\n");
	return ERR_OK;
}
#if LWIP_IPV6
err_t netif_output_ip6(struct netif* netif, struct pbuf* p, const ip6_addr_t* ipaddr)
{
	printf("netif_output_ip6\n");
}
#endif
err_t netif_linkoutput(struct netif* netif, struct pbuf* p)
{
	printf("netif_linkoutput\n");
	return ERR_OK;
}
err_t my_netif_init(struct netif* netif)
{
	printf("netif_init\n");
	default_interface.hwaddr_len = 6;
	default_interface.hwaddr[0] = 'm';
	default_interface.hwaddr[1] = 'a';
	default_interface.hwaddr[2] = 'c';
	default_interface.hwaddr[3] = 'a';
	default_interface.hwaddr[4] = 'd';
	default_interface.hwaddr[5] = 'r';
	default_interface.mtu = 1500;
	default_interface.name[0] = 'v'; // virtual
	default_interface.name[1] = 'i'; // interface
	default_interface.num = 0;
	default_interface.output = netif_output;
	default_interface.input = netif_input;
	default_interface.linkoutput = netif_linkoutput;
	default_interface.flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;
}


err_t example_client_connected(void *arg, struct tcp_pcb *tpcb, err_t err)
{
	printf("[CLIENT] Connected (err=%d)\n", err);
	fflush(stdout);
	//sys_thread_gate_open((sys_thread_gate_t*)arg);
}
void example_client(void* arg)
{
	err_t error;
	struct tcp_pcb* pcb;

	printf("[CLIENT] Starting...\n");

	pcb = tcp_new();
	if (!pcb) {
		FATAL("[CLIENT] tcp_new returned NULL");
		return 1;
	}

	{
		sys_thread_gate_t gate;
		ip4_addr_t addr;

		sys_thread_gate_new(&gate, 1, 0);
		//addr.addr = htonl(0x7F000001); // 127.0.0.1
		addr.addr = htonl(0xC0A80002); // 192.168.0.2

		printf("[CLIENT] connecting...\n");
		error = tcp_connect(pcb, &addr, 80, example_client_connected);

		sys_thread_gate_free(&gate);

	}

	if (error)
	{
		printf("[CLIENT] tcp_connect failed (e=%d)\n", error);
	}
	else
	{
		printf("[CLIENT] connected!\n");

	}

	error = tcp_close(pcb);
	if (error)
	{
		printf("[CLIENT] tcp_close failed (e=%d)\n", error);
	}
	printf("[CLIENT] Done\n");
}


int main(int argc, char* argv[])
{
	err_t error;
	struct tcp_pcb* pcb;

	//
	// Initialize the network stack
	//

	// initialize tcpip

	{
		sys_thread_gate_t gate;
		sys_thread_gate_new(&gate, 1, 0);

		tcpip_init_block(&gate);

		sys_thread_gate_free(&gate);
	}

	// install interfaces
	{
		struct netif* iface;

		ip4_addr_t address;
		ip4_addr_t netmask;
		ip4_addr_t gateway;
		address.addr = htonl(0xC0A80002); // 192.168.0.2
		netmask.addr = htonl(0xFFFFFF00);
		gateway.addr = htonl(0xC0A80001); // 192.168.0.1

		iface = netif_add(&default_interface,
			&address, &netmask, &gateway, NULL,
			my_netif_init, netif_input);
	}

	pcb = tcp_new();
	if (!pcb) {
		FATAL("tcp_new returned NULL");
		return 1;
	}

	error = tcp_bind(pcb, IP_ADDR_ANY, 80);
	if (error)
	{
		FATAL("tcp_bind failed");
		return 1;
	}

	{
		struct tcp_pcb* newpcb = tcp_listen(pcb);
		if (newpcb == NULL)
		{
			FATAL("tcp_listen failed");
			return 1;
		}
		printf("Original pcb: %p\n", pcb);
		printf("New pcb     : %p\n", newpcb);
		pcb = newpcb;
	}

	tcp_accept(pcb, accept_handler);



	//sys_thread_t sys_thread_new(const char *name, lwip_thread_fn thread, void *arg, int stacksize, int prio)
	sys_thread_new("Example Client", example_client, NULL, 0, 0);


	printf("Press Enter to stop\n");
	fflush(stdout);
	getchar();

	return 0;
}


