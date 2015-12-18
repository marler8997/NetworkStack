LWIPDIR=../../../lwip/src
LWIPARCHDIR=../mac

all: bin bin/lwip_netcat

bin/lwip_netcat: lwip_netcat.c ${LWIPARCHDIR}/lib/lwip.a
	gcc -o $@ -I ${LWIPDIR}/include -I ${LWIPARCHDIR} $^

bin:
	mkdir -p bin

clean:
	rm -rf bin
	rm -r *.o *~


