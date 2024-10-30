#include "Colors.h"
#include <Server.h>
#include <Maps.h>
#include <States.h>
#include <CMath.h>

#include <maps/HideAndSeek2.h>
#include <maps/RavineMist.h>
#include <maps/DotDotDot.h>
#include <maps/YouCantRun.h>
#include <maps/LimpCity.h>
#include <maps/NotPerfect.h>
#include <maps/KindAndFair.h>
#include <maps/Act9.h>
#include <maps/NastyParadise.h>
#include <maps/PricelessFreedom.h>
#include <maps/VolcanoValley.h>
#include <maps/Hill.h>
#include <maps/MajinForest.h>
#include <maps/TortureCave.h>
#include <maps/DarkTower.h>
#include <maps/HauntingDream.h>
#include <maps/FartZone.h>
#include <maps/WoodDream.h>
#include <maps/Marijuna.h>

Map g_mapList[MAP_COUNT+1] =
{
    { CLRCODE_GRN "Hide and Seek 2",	{ hs2_init, map_tick, map_tcpmsg, map_left }, 22 }, //0
    { CLRCODE_ORG "Ravine Mist",		{ rmz_init, rmz_tick, rmz_tcpmsg, rmz_left }, 27 }, //1
    { CLRCODE_GRN "...",				{ dot_init, map_tick, map_tcpmsg, map_left }, 25 }, //2
    { CLRCODE_ORG "Desert Town",		{ map_init, map_tick, map_tcpmsg, map_left }, 25 }, //3
    { CLRCODE_ORG "You Can't Run",		{ ycr_init, map_tick, map_tcpmsg, map_left }, 27 }, //4
    { CLRCODE_RED "Limp City",			{ lc_init,	map_tick, lc_tcpmsg,  map_left }, 23 }, //5
    { CLRCODE_RED "Not Perfect",		{ np_init,  map_tick, map_tcpmsg, map_left }, 59 }, //6
    { CLRCODE_ORG "Kind and Fair",		{ kaf_init, map_tick, kaf_tcpmsg, map_left }, 31 }, //7
    { CLRCODE_PUR "Act 9",				{ act9_init,map_tick, map_tcpmsg, map_left }, 38 }, //8
    { CLRCODE_RED "Nasty Paradise",		{ nap_init, map_tick, nap_tcpmsg, map_left }, 26 }, //9
    { CLRCODE_PUR "Priceless Freedom",	{ pf_init,	map_tick, pf_tcpmsg,  map_left }, 38 }, //10
    { CLRCODE_ORG "Volcano Valley",		{ vv_init,	map_tick, vv_tcpmsg,  map_left }, 27 }, //11
    { CLRCODE_ORG "Hill",				{ hill_init,map_tick, map_tcpmsg, map_left }, 26 }, //12
    { CLRCODE_PUR "Majin Forest",		{ maj_init, map_tick, map_tcpmsg, map_left }, 20 }, //13
    { CLRCODE_ORG "Hide and Seek",		{ map_init, map_tick, map_tcpmsg, map_left }, 21 }, //14
    { CLRCODE_RED "Torture Cave",		{ tc_init,  map_tick, map_tcpmsg, map_left }, 27 }, //15
    { CLRCODE_GRN "Dark Tower",			{ dt_init,  map_tick, dt_tcpmsg,  map_left }, 31 }, //16
    { CLRCODE_GRN "Haunting Dream",		{ hd_init,  map_tick, hd_tcpmsg,  map_left }, 31 }, //17
    { CLRCODE_RED "Mystic Wood",		{ wd_init,  map_tick, map_tcpmsg, map_left }, 26 }, //18
    { CLRCODE_GRN "Echidna Ruins",		{ mj_init,  map_tick, map_tcpmsg, map_left }, 28+5 }, //19
    { CLRCODE_BLU "Fart Zone",			{ ft_init,	map_tick, ft_tcpmsg,  map_left }, 15 }, //20
};

bool map_init(Server* server)
{
	RAssert(map_time(server, 3 * TICKSPERSEC, 20));
	RAssert(map_ring(server, 5));
	 
	return true;
}

bool map_tick(Server* server)
{
    if (server->game.time_sec <= g_config.ring_appearance_timer && server->game.bring_state < BS_DEACTIVATED && !g_config.disable_timer  && !g_config.overhell)
		game_bigring(server, BS_DEACTIVATED);

    if (server->game.time_sec <= g_config.escape_time && server->game.bring_state < BS_ACTIVATED  && !g_config.disable_timer  && !g_config.overhell)
		game_bigring(server, BS_ACTIVATED);
	
	return true;
}

bool map_tcpmsg(PeerData* v, Packet* packet)
{
	return true;
}

bool map_left(PeerData* v)
{
	return true;
}

bool map_time(Server* server, double seconds, float mul)
{
	server->game.time_sec = (uint16_t)(seconds + (((server_ingame(server) - 1) * mul)));
	return true;
}

bool map_ring(Server* server, int ringcoff)
{
	server->game.ring_coff = ringcoff;
	if (server_ingame(server) > 3)
		server->game.ring_coff--;
	
	if (server->game.ring_coff < 1)
		server->game.ring_coff = 1;
	
	return true;
}
