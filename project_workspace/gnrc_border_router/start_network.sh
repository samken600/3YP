#!/bin/sh

RIOTTOOLS="../RIOT/dist/tools/"

create_tap() {
    ip tuntap add ${TAP} mode tap user ${USER}
    sysctl -w net.ipv6.conf.${TAP}.forwarding=1
    sysctl -w net.ipv6.conf.${TAP}.accept_ra=0
    ip link set ${TAP} up
    ip a a fe80::1/64 dev ${TAP}
    ip a a fd00:dead:beef::1/128 dev lo
    ip route add ${PREFIX} via fe80::2 dev ${TAP}
}

remove_tap() {
    ip tuntap del ${TAP} mode tap
}

cleanup() {
    echo "Cleaning up..."
    # remove_tap
    ip a d fd00:dead:beef::1/128 dev lo
    ip a d fe80::1/64 dev ${TAP}
    ip route d ${PREFIX} via fe80::2 dev ${TAP}
    kill ${UHCPD_PID}
    trap "" INT QUIT TERM EXIT
}

config_if_routing() {
    sysctl -w net.ipv6.conf.${TAP}.forwarding=1
    sysctl -w net.ipv6.conf.${TAP}.accept_ra=0
    
    ip link set ${TAP} up
    ip a a fe80::1/64 dev ${TAP}
    ip a a fd00:dead:beef::1/128 dev lo
    ip route add ${PREFIX} via fe80::2 dev ${TAP}
    echo "Added route to ${PREFIX} on ${TAP}"
}

start_uhcpd() {
    ${UHCPD} ${TAP} ${PREFIX} > /dev/null &
    UHCPD_PID=$!
}

start_term() {
  TERMPROG="${RIOTTOOLS}/pyterm/pyterm"
  TERMFLAGS="-p "${PORT}" -b "${BAUDRATE}" ${PYTERMFLAGS}"
  ${TERMPROG} ${TERMFLAGS}

}

PORT=$1
TAP=$2
PREFIX=$3
BAUDRATE=115200
UHCPD="${RIOTTOOLS}/uhcpd/bin/uhcpd"

[ -z "${PORT}" -o -z "${TAP}" -o -z "${PREFIX}" ] && {
    echo "usage: $0 <serial-port> <tap-device> <prefix> [baudrate]"
    exit 1
}

[ ! -z $4 ] && {
    BAUDRATE=$4
}

trap "cleanup" INT QUIT TERM EXIT


# create_tap && start_uhcpd && "${ETHOS_DIR}/ethos" ${TAP} ${PORT} ${BAUDRATE}
config_if_routing && start_uhcpd && start_term
#start_term
