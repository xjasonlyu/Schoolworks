package main

import (
	"fmt"
	"log"
	"os"
	"time"

	"github.com/google/gopacket"
	"github.com/google/gopacket/layers"
	"github.com/google/gopacket/pcap"
	"github.com/google/gopacket/pcapgo"
)

const (
	filter     = "ip4"
	deviceName = "en0"
	sampleName = "sample.pcap"
	snapLen    = 1600
	timeout    = 2 * time.Minute
)

func main() {
	if _, err := os.Stat(sampleName); err != nil {
		capture()
	}

	handle, err := pcap.OpenOffline(sampleName)
	if err != nil {
		log.Fatal(err)
	}
	defer handle.Close()

	// count := 0
	// Loop through packets in file
	packetSource := gopacket.NewPacketSource(handle, handle.LinkType())
	for packet := range packetSource.Packets() {
		//count++
		//if count > 2 {
		//	break
		//}

		if packet.Layer(layers.LayerTypeIPv4).(*layers.IPv4).Protocol == 17 {
			printPacketInfo(packet)
		}

	}
}

func printPacketInfo(packet gopacket.Packet) {
	// Check for errors
	if err := packet.ErrorLayer(); err != nil {
		fmt.Println("Error decoding some part of the packet:", err)
	}

	const format = `@Information:
	时间戳：%d
	源地址：%s
	目的地址：%s
	协议：%s (%d)
	总长度：%d
	TTL：%d
	df：%d
	mf：%d
	offset：%d
	校验码：%#x (%d)
`

	ipLayer := packet.Layer(layers.LayerTypeIPv4)
	if ip, ok := ipLayer.(*layers.IPv4); ok {
		fmt.Printf(format,
			packet.Metadata().Timestamp.Nanosecond(),
			ip.SrcIP,
			ip.DstIP,
			ip.Protocol, ip.Protocol,
			ip.Length,
			ip.TTL,
			(ip.Flags&layers.IPv4DontFragment)>>1,
			ip.Flags&layers.IPv4MoreFragments,
			ip.FragOffset,
			ip.Checksum, ip.Checksum,
		)
	}
}

func capture() {
	f, err := os.Create(sampleName)
	if err != nil {
		log.Fatal(err)
	}

	w := pcapgo.NewWriter(f)
	_ = w.WriteFileHeader(snapLen, layers.LinkTypeEthernet)
	defer f.Close()

	// Open the device for capturing
	handle, err := pcap.OpenLive(deviceName, snapLen, true, timeout)
	if err != nil {
		log.Fatalf("Error opening device %s: %v", deviceName, err)
	}
	defer handle.Close()

	_ = handle.SetBPFFilter(filter)

	timer := time.After(timeout)
	// Start processing packets
	log.Println("Start capturing...")
	packetSource := gopacket.NewPacketSource(handle, handle.LinkType())
	packets := packetSource.Packets()
	for {
		select {
		case <-timer:
			goto out
		case packet := <-packets:
			// log.Println(packet)
			_ = w.WritePacket(packet.Metadata().CaptureInfo, packet.Data())
		}
	}

out:
	log.Println("Capture completed")
}
