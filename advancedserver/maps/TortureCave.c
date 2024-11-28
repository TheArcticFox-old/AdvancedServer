#include <maps/TortureCave.h>
#include <entities/TCAcid.h>

bool tc_init(Server* server)
{
	RAssert(map_time(server, 2.585 * TICKSPERSEC, 20)); //155
	RAssert(map_ring(server, 5));
    RAssert(game_spawn(server, (Entity*)&(MakeAcid()), sizeof(Acid), NULL));
	return true;
}
