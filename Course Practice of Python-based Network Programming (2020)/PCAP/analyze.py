import os
import sys

from enum import Enum

from scapy.utils import RawPcapReader
from scapy.layers.l2 import Ether
from scapy.layers.inet import IP, TCP

import matplotlib.pyplot as plt

client_ip, client_port = '10.66.246.221', 51855
server_ip, server_port = '192.168.102.15', 80


class PktDirection(Enum):
    not_defined = 0
    client_to_server = 1
    server_to_client = 2


def draw(data):
    ax = plt.gca()
    for item in ax.spines.values():
        item.set_color("none")
    plt.title('TCP handshake process', fontsize=18)

    fmt = 'flag={flag:<3s} seq={seq:<9d} ack={ack:<9d} len={len:<6d}'

    plt.axis([0, 20, 0, 20])
    plt.xticks([0, 20], [f"Client\n{client_ip}",
                         f"Server\n{server_ip}"])
    plt.yticks([])
    plt.vlines(x=0, ymin=0, ymax=20, colors="black")
    plt.vlines(x=20, ymin=0, ymax=20, colors="black")

    plt.arrow(0, 19, 20, -5, length_includes_head=True, width=0.2, ec='red')
    plt.text(10, 17, '(1) '+fmt.format(**
                                       data[0]), horizontalalignment='center')

    plt.arrow(20, 13, -20, -5, length_includes_head=True, width=0.2, ec='blue')
    plt.text(10, 11, '(2) '+fmt.format(**
                                       data[1]), horizontalalignment='center')

    plt.arrow(0, 7, 20, -5, length_includes_head=True, width=0.2, ec='green')
    plt.text(10, 5, '(3) '+fmt.format(**data[2]), horizontalalignment='center')

    plt.show()


def process_pcap(file_name):
    print(f"Opening {file_name}...")

    count = 0
    tcp_packet_count = 0

    server_sequence_offset = None
    client_sequence_offset = None

    data = []

    print('###########################################################################')
    print('TCP session between client {}:{} and server {}:{}'.
          format(client_ip, client_port, server_ip, server_port))
    print('###########################################################################')

    for pkt_data, pkt_metadata in RawPcapReader(file_name):
        count += 1

        ether_pkt = Ether(pkt_data)
        if 'type' not in ether_pkt.fields:
            # LLC frames will have 'len' instead of 'type'.
            # We disregard those
            continue

        if ether_pkt.type != 0x0800:
            # disregard non-IPv4 packets
            continue

        ip_pkt = ether_pkt[IP]
        if ip_pkt.proto != 6:
            # Ignore non-TCP packet
            continue

        if ip_pkt.src != server_ip and ip_pkt.src != client_ip:
            continue

        if ip_pkt.dst != server_ip and ip_pkt.dst != client_ip:
            continue

        # print(ip_pkt.src)

        tcp_pkt = ip_pkt[TCP]

        if tcp_pkt.sport != server_port and tcp_pkt.sport != client_port:
            continue

        if tcp_pkt.dport != server_port and tcp_pkt.dport != client_port:
            continue

        direction = PktDirection.not_defined
        if (ip_pkt.src, tcp_pkt.sport) == (client_ip, client_port) and (ip_pkt.dst, tcp_pkt.dport) == (server_ip, server_port):
            direction = PktDirection.client_to_server
        elif (ip_pkt.src, tcp_pkt.sport) == (server_ip, server_port) and (ip_pkt.dst, tcp_pkt.dport) == (client_ip, client_port):
            direction = PktDirection.server_to_client
        else:
            continue

        # print(pkt_metadata)

        tcp_packet_count += 1
        if tcp_packet_count == 1:
            first_pkt_timestamp = (pkt_metadata.tshigh <<
                                   32) | pkt_metadata.tslow
            first_pkt_timestamp_resolution = pkt_metadata.tsresol
            first_pkt_ordinal = count

        last_pkt_timestamp = (pkt_metadata.tshigh << 32) | pkt_metadata.tslow
        last_pkt_timestamp_resolution = pkt_metadata.tsresol
        last_pkt_ordinal = count

        this_pkt_relative_timestamp = last_pkt_timestamp - first_pkt_timestamp

        if direction == PktDirection.client_to_server:
            if client_sequence_offset is None:
                client_sequence_offset = tcp_pkt.seq
            relative_offset_seq = tcp_pkt.seq - client_sequence_offset
        else:
            assert direction == PktDirection.server_to_client
            if server_sequence_offset is None:
                server_sequence_offset = tcp_pkt.seq
            relative_offset_seq = tcp_pkt.seq - server_sequence_offset

        # If this TCP packet has the Ack bit set, then it must carry an ack
        # number.
        if 'A' not in str(tcp_pkt.flags):
            relative_offset_ack = 0
        else:
            if direction == PktDirection.client_to_server:
                relative_offset_ack = tcp_pkt.ack - server_sequence_offset
            else:
                relative_offset_ack = tcp_pkt.ack - client_sequence_offset

        # Determine the TCP payload length. IP fragmentation will mess up this
        # logic, so first check that this is an unfragmented packet
        if (ip_pkt.flags == 'MF') or (ip_pkt.frag != 0):
            print('No support for fragmented IP packets')
            break

        tcp_payload_len = ip_pkt.len - (ip_pkt.ihl * 4) - (tcp_pkt.dataofs * 4)

        # Print
        fmt = '[{ordnl:>5}]{ts:>10.6f}s flag={flag:<3s} seq={seq:<9d} ack={ack:<9d} len={len:<6d}'

        if direction == PktDirection.client_to_server:
            fmt = '{arrow}' + fmt
            arr = '-->'
        else:
            fmt = '{arrow:>69}' + fmt
            arr = '<--'

        flag = str(tcp_pkt.flags)
        if str(tcp_pkt.flags) == 'A':
            flag = 'ACK'
        elif str(tcp_pkt.flags) == 'S':
            flag = 'SYN'
        elif str(tcp_pkt.flags) == 'SA':
            flag = 'SYN ACK'

        param = dict(arrow=arr,
                     ordnl=last_pkt_ordinal,
                     ts=this_pkt_relative_timestamp / pkt_metadata.tsresol,
                     flag=flag,
                     seq=relative_offset_seq,
                     ack=relative_offset_ack,
                     len=tcp_payload_len)

        print(fmt.format(**param))

        data.append(param)

        if tcp_packet_count == 3:
            break

    assert len(data) == 3

    draw(data)

    # print(f'{tcp_packet_count} of {count} packets in total')


def main():
    if len(sys.argv) != 2:
        print(f"Usage: {sys.argv[0]} <pcapng file>")
        sys.exit(0)

    file_name = sys.argv[1]
    if not os.path.isfile(file_name):
        print(f'"{file_name}" does not exist', file=sys.stderr)
        sys.exit(1)

    process_pcap(file_name)


if __name__ == "__main__":
    main()
