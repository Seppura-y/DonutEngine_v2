#ifndef PACKET_WARPPER_H
#define PACKET_WARPPER_H

#include <memory>

struct AVPacket;

class PacketWarpper
{
public:
	PacketWarpper();
	~PacketWarpper();
private:
	std::unique_ptr<AVPacket*> packet_;
};
#endif