LWIPDIR=../../../lwip/src
include ${LWIPDIR}/Filelists.mk

compile_cmd=gcc -c -o $@ -I . -I ${LWIPDIR}/include $^

BUILD_DIRS = lib obj obj/core obj/core/ipv4 obj/core/ipv6 obj/api obj/netif obj/netif/ppp obj/netif/ppp/polarssl


#COREOBJS  = $(patsubst ${LWIPDIR}/%.c,obj/%.o,${COREFILES})
#CORE4OBJS = $(patsubst ${LWIPDIR}/%.c,obj/%.o,${CORE4FILES})
#CORE6OBJS = $(patsubst ${LWIPDIR}/%.c,obj/%.o,${CORE6FILES})
#APIOBJS   = $(patsubst ${LWIPDIR}/%.c,obj/%.o,${APIFILES})
LWIPNOAPPSOBJS = $(patsubst ${LWIPDIR}/%.c,obj/%.o,${LWIPNOAPPSFILES})

all: ${BUILD_DIRS} obj/sys_arch.o lib/lwip.a

lib/lwip.a: obj/sys_arch.o ${LWIPNOAPPSOBJS}
	ar rcs $@ $^

${BUILD_DIRS}:
	mkdir -p ${BUILD_DIRS}

obj/sys_arch.o: sys_arch.c
	gcc -c -o $@ -I . -I ${LWIPDIR}/include $^

obj/%.o: ${LWIPDIR}/%.c
	gcc -c -o $@ -I . -I ${LWIPDIR}/include $^

clean:
	rm -rf obj lib
	rm -f *~ arch/*~
