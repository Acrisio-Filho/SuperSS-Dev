// Arquivo packet_func_sv.h
// Criado em 13/08/2017 por Acrisio
// Definição da classe packet_func para o Game Server

#pragma once
#ifndef _STDA_PACKET_FUNC_SV_H
#define _STDA_PACKET_FUNC_SV_H

#if defined(_WIN32)
#include <Windows.h>
#include <WinSock2.h>
#elif defined(__linux__)
#include "../../Projeto IOCP/UTIL/WinPort.h"
#endif

#include "../../Projeto IOCP/TYPE/stdAType.h"
#include "../../Projeto IOCP/PACKET/packet_func.h"
#include "../TYPE/pangya_game_st.h"
#include "../../Projeto IOCP/PACKET/packet.h"
#include "../GAME/channel.h"
#include <string>
#include <vector>
#include <map>

#include "../TYPE/player_info.h"
#include "../SESSION/player.hpp"

#define MAKE_BEGIN_SERVER(_arg1) game_server *gs = reinterpret_cast< game_server* >((_arg1));

#define _MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2) MAKE_BEGIN_SERVER(_arg1) _MAKE_BEGIN_PACKET(_arg2)

#define MAKE_BEGIN_PACKET_AUTH_SERVER(_arg1, _arg2) MAKE_BEGIN_SERVER(_arg1) _MAKE_BEGIN_PACKET_AUTH_SERVER(_arg2)

#ifdef _DEBUG
#define MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2) _MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2) _MSG_BEGIN_PACKET
#else
#define MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2) _MAKE_BEGIN_PACKET_SERVER(_arg1, _arg2)
#endif

namespace stdA {
    class packet_func : public packet_func_base {
        public:
            // cliente
            static int packet002(void* _arg1, void* _arg2);	// Login
			static int packet003(void* _arg1, void* _arg2);	// Msg player
			static int packet004(void* _arg1, void* _arg2);	// Entra canal
			static int packet006(void* _arg1, void* _arg2);	// [Game] request::Finish Game
			static int packet007(void* _arg1, void* _arg2);	// Check Nick
			static int packet008(void* _arg1, void* _arg2);	// Cria sala
			static int packet009(void* _arg1, void* _arg2);	// Entra na sala
			static int packet00A(void* _arg1, void* _arg2);	// Troca info da sala
			static int packet00B(void* _arg1, void* _arg2);	// Att Item no Canal Ex: Character
			static int packet00C(void* _arg1, void* _arg2);	// Att Item na Sala Ex: Character
			static int packet00D(void* _arg1, void* _arg2); // Att Estado do player na sala
			static int packet00E(void* _arg1, void* _arg2);	// [Game] request::Começa Jogo
			static int packet00F(void* _arg1, void* _arg2);	// Sai da sala
			static int packet010(void* _arg1, void* _arg2);	// Troca o team do player na sala
			static int packet011(void* _arg1, void* _arg2);	// [Game] request::Finish Load Hole
			static int packet012(void* _arg1, void* _arg2);	// [Game] request::Init Shot
			static int packet013(void* _arg1, void* _arg2);	// [Game] request::Change Mira
			static int packet014(void* _arg1, void* _arg2);	// [Game] request::Change State Bar Space
			static int packet015(void* _arg1, void* _arg2);	// [Game] request::Active Power Shot
			static int packet016(void* _arg1, void* _arg2);	// [Game] request::Change Club
			static int packet017(void* _arg1, void* _arg2);	// [Game] request::Use Active Item
			static int packet018(void* _arg1, void* _arg2);	// [Game] request::Change State Typeing "Escrevendo"
			static int packet019(void* _arg1, void* _arg2);	// [Game] request::Move Ball
			static int packet01A(void* _arg1, void* _arg2);	// [Game] request::Inicializa o Hole
			static int packet01B(void* _arg1, void* _arg2);	// [Game] request::Sync Shot
			static int packet01C(void* _arg1, void* _arg2); // [Game] request::Finish Shot
			static int packet01D(void* _arg1, void* _arg2);	// [Shop] Compra Item Shop
			static int packet01F(void* _arg1, void* _arg2);	// [Shop] Presente para um outro player
			static int packet020(void* _arg1, void* _arg2);	// Update Item equipado
			static int packet022(void* _arg1, void* _arg2);	// [Game] request::Start Turn Time
			static int packet026(void* _arg1, void* _arg2);	// Chuta(Kick) o player da sala
			static int packet029(void* _arg1, void* _arg2);	// [Invite Room] request:: Invite Room Info, algo do tipo, o Original o server num retorna nada
			static int packet02A(void* _arg1, void* _arg2);	// [Private Message] request:: Private Message
			static int packet02D(void* _arg1, void* _arg2);	// Info Sala
			static int packet02F(void* _arg1, void* _arg2);	// Info Player
			static int packet030(void* _arg1, void* _arg2);	// [Game] request::Pause Or UnPause Game
			static int packet031(void* _arg1, void* _arg2);	// [Game] request::Finish Hole Data
			static int packet032(void* _arg1, void* _arg2);	// Troca o estado do Player na sala(AFK or normal)
			static int packet033(void* _arg1, void* _arg2);	// Client send exception message of crash client
			static int packet034(void* _arg1, void* _arg2);	// [Game] request::Finish Char Intro
			static int packet035(void* _arg1, void* _arg2);	// [Game] request Team Finish Hole
			static int packet036(void* _arg1, void* _arg2);	// [Game] request:: Resposta se quer continuar o Versus
			static int packet037(void* _arg1, void* _arg2);	// [Game] request::Finish VERSUS Last Player On VERSUS
			static int packet039(void* _arg1, void* _arg2);	// [Caddie Pay Day Holy::Paga Ferias do Caddie] request::PayHolyDay
			static int packet03A(void* _arg1, void* _arg2);	// [Game] request::Report Game Chat

			// 2018-03-04 19:26:39.633	Tipo: 60(0x3C), desconhecido ou nao implementado. func_arr::getPacketCall()	 Error Code: 335609856
			// 2018-03-04 19:26:39.633	size packet: 4
			//
			//0000 3C 00 1F 01 -- -- -- -- -- -- -- -- -- -- -- -- 	<...............
			//static int packet03C(void* _arg1, void* _arg2);	// manda msg OFF na opção 0x6F e a opção 0x11F pede a lista de amigos para enviar presente
			
			static int packet03C(void* _arg1, void* _arg2);	// [Sub Packet Request]. 0x111[MSG OFF], 0x11F[LIST FRIEND]
			static int packet03D(void* _arg1, void* _arg2);	// [Cookie] request Cookies
			static int packet03E(void* _arg1, void* _arg2); // [Game][GM::Spy] request::EnterSpyRoom(Observer)
			static int packet041(void* _arg1, void* _arg2);	// [Command GM] request::Identity
			static int packet042(void* _arg1, void* _arg2);	// [Game] request::Init Shot Arrow Sequence
			static int packet043(void* _arg1, void* _arg2);	// [Server] Pede Server List
			static int packet047(void* _arg1, void* _arg2);	// [Rank] request::Rank Server
			static int packet048(void* _arg1, void* _arg2);	// [Game] request::LoadGamePercent
			static int packet04A(void* _arg1, void* _arg2);	// [Game] request::Active Replay
			static int packet04B(void* _arg1, void* _arg2);	// [UpdateStats(ClubSet/Character*)] request::UpdateStats, *Character só nas season's passadas
			static int packet04F(void* _arg1, void* _arg2);	// [Game] request::Change State Chat Block
			static int packet054(void* _arg1, void* _arg2);	// [Game] request::Chat Team
			static int packet055(void* _arg1, void* _arg2);	// [Private Message] request::Change Whisper State
			static int packet057(void* _arg1, void* _arg2);	// [Command GM] request::Notice GM
			static int packet05C(void* _arg1, void* _arg2);	// [Server] request::Server DateTime
			// 2018 - 12 - 01 18:49 : 14.928 size packet : 4
			// Destroy Room, 2 Bytes Room Number
			// 0000 60 00 01 00 -- -- -- -- -- -- -- -- -- -- -- --    `...............
			static int packet060(void* _arg1, void* _arg2);	// [Command GM] request::Destroy Room(num)
			// 2018 - 12 - 01 18:48 : 02.634 size packet : 6
			// Disconnect User, 2 Bytes Online ID
			// 0000 61 00 00 00 00 00 -- -- -- -- -- -- -- -- -- --a...............
			static int packet061(void* _arg1, void* _arg2);	// [Command GM] request::Disconnect User(oid)
			static int packet063(void* _arg1, void* _arg2);	// Att Coordenada do Player na sala
			static int packet064(void* _arg1, void* _arg2);	// [DeleteActiveItem] request::DeleteActiveItem
			static int packet065(void* _arg1, void* _arg2);	// [Game] request::Active Booster
			static int packet066(void* _arg1, void* _arg2);	// [Ticker] request::Send Ticker
			static int packet067(void* _arg1, void* _arg2);	// [Ticker] request::File Ticker Dados
			static int packet069(void* _arg1, void* _arg2);	// [ChatMacro] request::ChangeChatMacroUser
			static int packet06B(void* _arg1, void* _arg2);	// [Caddie] request::Set Notice Caddie Holy Day
			static int packet073(void* _arg1, void* _arg2);	// [MyRoom] request::Change Message Mascot
			static int packet074(void* _arg1, void* _arg2);	// [Personal Shop] request::CancelEditSaleShop
			static int packet075(void* _arg1, void* _arg2);	// [Personal Shop] request::CloseSaleShop
			static int packet076(void* _arg1, void* _arg2);	// [Personal Shop] request::OpenEditSaleShop
			static int packet077(void* _arg1, void* _arg2);	// [Personal Shop] request::ViewSaleShop
			static int packet078(void* _arg1, void* _arg2);	// [Personal Shop] request::CloseViewSaleShop
			static int packet079(void* _arg1, void* _arg2);	// [Personal Shop] request::ChangeNameSaleShop
			static int packet07A(void* _arg1, void* _arg2);	// [Personal Shop] request::VisitCount
			static int packet07B(void* _arg1, void* _arg2);	// [Personal Shop] request::PangSale
			static int packet07C(void* _arg1, void* _arg2);	// [Personal Shop] request::OpenSaleShop
			static int packet07D(void* _arg1, void* _arg2);	// [Personal Shop] request::BuyItemSaleShop
			static int packet081(void* _arg1, void* _arg2);	// Entra na lobby multiplayer
			static int packet082(void* _arg1, void* _arg2); // Sai da lobby multiplayer
			static int packet083(void* _arg1, void* _arg2);	// Troca de canal apartir da lobby
			static int packet088(void* _arg1, void* _arg2);	// [GameGuard] Reply GameGuard Auth
			static int packet08B(void* _arg1, void* _arg2);	// Pede Msn Server
			static int packet08F(void* _arg1, void* _arg2);	// [Command GM] request::Command GM
			static int packet098(void* _arg1, void* _arg2);	// [Papel Shop] request::Open Papel Shop
			static int packet09C(void* _arg1, void* _arg2);	// Pede os ultimos 5 Players que Jogou
			static int packet09D(void* _arg1, void* _arg2);	// [Game] request::EnterGameAfterStarted
			static int packet09E(void* _arg1, void* _arg2);	// [Gacha Coupon] request::UpdateGachaCoupon
			static int packet0A1(void* _arg1, void* _arg2);	// [Web Link] request Enter Web Link 1 byte qual web link
			static int packet0A2(void* _arg1, void* _arg2);	// [Web Link] request Quit Web Link. (Só vi ele sendo utilizado no Guild WebLink)
			static int packet0AA(void* _arg1, void* _arg2);	// [Game] request::Use Ticket Report ON Tourney
			static int packet0AB(void* _arg1, void* _arg2);	// [Ticket Report Scroll] request::Open Ticket Report Scroll
			static int packet0AE(void* _arg1, void* _arg2);	// [Tutorial] request::MakeTutorial
			static int packet0B2(void* _arg1, void* _arg2);	// [Box System] request::Open Box MyRoom
			static int packet0B4(void* _arg1, void* _arg2);	// Ele manda o número da sala que ele foi convidado e aceito e depois relogou com o game server, aí ele manda esse pacote com o número da sala
			static int packet0B5(void* _arg1, void* _arg2);	// Pede para entrar no my room do player
			static int packet0B7(void* _arg1, void* _arg2);	// Pede o character equipado do player no my room
			static int packet0B9(void* _arg1, void* _arg2);	// [UCC::Self Design System] request:: Info, [Salve Temporario ou eterno]
			static int packet0BA(void* _arg1, void* _arg2);	// [Room:: Invite Player] request::Invite Player
			static int packet0BD(void* _arg1, void* _arg2);	// [CardSpecial] request::Use card special
			static int packet0C1(void* _arg1, void* _arg2);	// Att Place(lugar) do player
			static int packet0C9(void* _arg1, void* _arg2);	// [UCC::Self Design System] request::Key For Save UCC(temporario ou para sempre)
			static int packet0CA(void* _arg1, void* _arg2);	// [CardSystem] request::Open Card Pack
			static int packet0CB(void* _arg1, void* _arg2);	// [Game::Approach] request::FinishGame
			static int packet0CC(void* _arg1, void* _arg2);	// [Dolfini Locker] request::Check Pass Enter Dolfini Locker
			static int packet0CD(void* _arg1, void* _arg2);	// [Dolfini Locker] request::Item(ns)
			static int packet0CE(void* _arg1, void* _arg2);	// [Dolfini Locker] request::Add Item
			static int packet0CF(void* _arg1, void* _arg2);	// [Dolfini Locker] request::Remove Item
			static int packet0D0(void* _arg1, void* _arg2);	// [Dolfini Locker] request::Cria senha
			static int packet0D1(void* _arg1, void* _arg2);	// [Dolfini Locker] request::Trocar senha
			static int packet0D2(void* _arg1, void* _arg2);	// [Dolfini Locker] request::Trocar Modo de Entrar, se precisa de senha ou nao
			static int packet0D3(void* _arg1, void* _arg2);	// [Dolfini Locker] request::Check if have Dolfini Locker Password(Verifica se tem dolfini locker senha)
			static int packet0D4(void* _arg1, void* _arg2);	// [Dolfini Locker] request::Update Pang
			static int packet0D5(void* _arg1, void* _arg2);	// [Dolfini Locker] request::Pang
			static int packet0D8(void* _arg1, void* _arg2);	// [Item Buff] request::Use item buff
			static int packet0DE(void* _arg1, void* _arg2);	// player não pode ver a mensagem que o player(nickname) mandou para ele, o player está avisando para o server que ele não pode ver a mensagem.
			static int packet0E5(void* _arg1, void* _arg2);	// [Game] request::Active Cutin
			static int packet0E6(void* _arg1, void* _arg2);	// [Rental] request::Extend
			static int packet0E7(void* _arg1, void* _arg2);	// [Rental] request::Delete
			static int packet0EB(void* _arg1, void* _arg2);	// Pede o Estado do Character Equipado do player no lounge
			static int packet0EC(void* _arg1, void* _arg2);	// [CometRefill] request:: Refill Comet
			static int packet0EF(void* _arg1, void* _arg2);	// [Box System] request::Open Box MailBox
			static int packet0F4(void* _arg1, void* _arg2); // TTL(Time to Live) heartbeat
			static int packet0FB(void* _arg1, void* _arg2);	// Web Key Auth
			static int packet0FE(void* _arg1, void* _arg2); // Pede o pacote1B1 com uns valores que ainda não sei direito
			static int packet119(void* _arg1, void* _arg2);	// Troca de server
			static int packet126(void* _arg1, void* _arg2);	// [Tiki Shop(PointShop)] request::requestOpenPointShop
			static int packet127(void* _arg1, void* _arg2);	// [Tiki Shop(PointShop)] request::requestPointShop Points
			static int packet128(void* _arg1, void* _arg2);	// [Tiki Shop(PointShop)] request::requestExchangeTPByItem
			static int packet129(void* _arg1, void* _arg2);	// [Tiki Shop(PointShop)] request::requestExchangeItemByTP
			static int packet12C(void* _arg1, void* _arg2);	// [Game] request::Finish Grand Zodiac game
			static int packet12D(void* _arg1, void* _arg2);	// [Game] request::Reply Camera Initialize Vector Grand Zodiac(pacote1EC)
			static int packet12E(void* _arg1, void* _arg2);	// [Game] request::Marker On Course
			static int packet12F(void* _arg1, void* _arg2);	// [Game] request::Shot End Location Data
			static int packet130(void* _arg1, void* _arg2);	// [Game] request::Exit Practice
			static int packet131(void* _arg1, void* _arg2);	// [Game] request::Exit Chip-in Practice
			static int packet137(void* _arg1, void* _arg2);	// [Game] request::Start First Hole Grand Zodiac
			static int packet138(void* _arg1, void* _arg2);	// [Game] request::Active Wing
			static int packet140(void* _arg1, void* _arg2);	// Quando entra no shop manda esse pacote
			static int packet141(void* _arg1, void* _arg2);	// [Game] request::Change Wind Next Hole Repeat
			static int packet143(void* _arg1, void* _arg2); // [MailBox] Abre o MailBox
			static int packet144(void* _arg1, void* _arg2); // [MailBox] Info Email
			static int packet145(void* _arg1, void* _arg2);	// [MailBox] Send Email With/out Item
			static int packet146(void* _arg1, void* _arg2); // [MailBox] Move Item Para o Armario
			static int packet147(void* _arg1, void* _arg2);	// [MailBox] Exclui Email
			static int packet14B(void* _arg1, void* _arg2);	// [Papel Shop] request::Play Papel Shop Normal
			static int packet151(void* _arg1, void* _arg2); // [DailyQuest] Abre DailyQuest
			static int packet152(void* _arg1, void* _arg2);	// [DailyQuest] Aceite DailyQuest
			static int packet153(void* _arg1, void* _arg2); // [DailyQuest] Request pega recompensa da quest
			static int packet154(void* _arg1, void* _arg2);	// [DailyQuest] Desiste da(s) Quest aceita anteriormente
			static int packet155(void* _arg1, void* _arg2); // [CardSystem::LoloCardCompose] request::Card Compose
			static int packet156(void* _arg1, void* _arg2);	// [Game] request::Active Auto Command
			static int packet157(void* _arg1, void* _arg2); // [Achievement] request::Abre Achievement
			static int packet158(void* _arg1, void* _arg2);	// [CadieMagicBox] request::Exchange Item
			static int packet15C(void* _arg1, void* _arg2);	// [Game] request::Active Paws
			static int packet15D(void* _arg1, void* _arg2);	// [Game] request::Active Ring	[Esse é o Pacote de 1 Anel]
			static int packet164(void* _arg1, void* _arg2);	// [ClubSet Workshop] request::Up Level
			static int packet165(void* _arg1, void* _arg2);	// [ClubSet Workshop] request::Up Level Confirm
			static int packet166(void* _arg1, void* _arg2);	// [ClubSet Workshop] request::Up Level Cancel
			static int packet167(void* _arg1, void* _arg2);	// [ClubSet Workshop] request::Up Rank
			static int packet168(void* _arg1, void* _arg2);	// [ClubSet Workshop] request::Up Rank Transform Confirm
			static int packet169(void* _arg1, void* _arg2);	// [ClubSet Workshop] request::Up Rank Transform Cancel
			static int packet16B(void* _arg1, void* _arg2);	// [ClubSet Workshop] request::Recovery Pts
			static int packet16C(void* _arg1, void* _arg2);	// [ClubSet Workshop] request::Transfer Mastery Pts
			static int packet16D(void* _arg1, void* _arg2);	// [ClubSet Reset] request::ClubSet Reset
			static int packet16E(void* _arg1, void* _arg2); // [AttendenceReward] Abre AttendenceReward
			static int packet16F(void* _arg1, void* _arg2); // [AttendenceReward] Counta o AttendenceReward do dia e abre ele
			static int packet171(void* _arg1, void* _arg2);	// [Game] request::Active Earcuff
			static int packet172(void* _arg1, void* _arg2);	// [Event] request::OpenEventWorkshop2013
			static int packet176(void* _arg1, void* _arg2);	// [Grand Prix] request Enter Lobby Grand Prix
			static int packet177(void* _arg1, void* _arg2);	// [Grand Prix] request Leave Lobby Grand Prix
			static int packet179(void* _arg1, void* _arg2);	// [Grand Prix] request Enter Room Grand Prix
			static int packet17A(void* _arg1, void* _arg2);	// [Grand Prix] request Leave Room Grand Prix
			static int packet17F(void* _arg1, void* _arg2);	// [Memorial System] request::Play Memorial
			static int packet180(void* _arg1, void* _arg2);	// [Game] request::Active Glove
			static int packet181(void* _arg1, void* _arg2);	// [Game] request::Active Ring Terreno [Esse é o Pacote de combinação de 2 Aneis]
			static int packet184(void* _arg1, void* _arg2);	// [Assist] request::Toggle Assist
			static int packet185(void* _arg1, void* _arg2);	// [Assist] request::Active Assist Green
			static int packet186(void* _arg1, void* _arg2);	// [Papel Shop] request::Play Papel Shop Big
			static int packet187(void* _arg1, void* _arg2);	// [CharacterStats] request::Expand Mastery
			static int packet188(void* _arg1, void* _arg2);	// [CharacterStats] request::Upgrade Stats
			static int packet189(void* _arg1, void* _arg2);	// [CharacterStats] request::Downgrade Stats
			static int packet18A(void* _arg1, void* _arg2);	// [CharacterStats] request::Equip Card
			static int packet18B(void* _arg1, void* _arg2);	// [CharacterStats] request::Equip Card With Club Patcher
			static int packet18C(void* _arg1, void* _arg2);	// [CharacterStats] request::Desequip Card
			static int packet18D(void* _arg1, void* _arg2);	// [TikiShop]	request::Exchange Item
			static int packet192(void* _arg1, void* _arg2);	// [Event] request::OpenEventArin2014
			static int packet196(void* _arg1, void* _arg2);	// [Game] request::Active Ring Paws Rainbow JP
			static int packet197(void* _arg1, void* _arg2);	// [Game] request::Active Ring Power Gague JP
			static int packet198(void* _arg1, void* _arg2);	// [Game] request::Active Ring Miracle Sign JP
			static int packet199(void* _arg1, void* _arg2);	// [Game] request::Active Ring Paws Ring Set JP

			// packet server
			static int packet_sv4D(void* _arg1, void* _arg2);
			static int packet_sv055(void* _arg1, void* _arg2);
			static int packet_svRequestInfo(void* _arg1, void* _arg2);
			static int packet_sv22D(void* _arg1, void* _arg2);
			static int packet_svFazNada(void* _arg1, void* _arg2);
			static int packet_svDisconectPlayerBroadcast(void* _arg1, void* _arg2);

			// packet Auth Server
			static int packet_as001(void* _arg1, void* _arg2);

            // server
			static int						pacote040(packet& p, player *_session, PlayerInfo *pi, std::string msg, unsigned char option = 0);
            static int						pacote044(packet& p, player *_session, ServerInfoEx& _si, int option = 0, PlayerInfo *pi = nullptr, int valor = 0);
			static int						pacote046(packet& p, player *_session, std::vector< PlayerCanalInfo > v_element, int option = 0);
			static int						pacote047(packet& p, std::vector< RoomInfo > v_element, int option = 0);
			static int						pacote048(packet& p, player *_session, std::vector< PlayerRoomInfoEx > v_element, int option = 0);
			static int						pacote049(packet& p, room *_room, int option = 0);
			static int						pacote04A(packet& p, RoomInfoEx& _ri, int option);
			static int						pacote04B(packet& p, player *_session, unsigned char _type, int error = 0, int _valor = 0);
			static int						pacote04C(packet& p, player *_session, int option = 0);
			static int						pacote04D(packet& p, player *_session, std::vector< channel* >& v_element, int option = 0);
			static int						pacote04E(packet& p, player *_session, int option, int _codeErrorInfo = 0);
			static int						pacote06B(packet& p, player *_session, PlayerInfo *pi, unsigned char type, unsigned char err_code = 4);
			static int						pacote070(packet& p, player *_session, std::multimap< int32_t/*ID*/, CharacterInfo >& v_element, int option = 0);
			static int						pacote071(packet& p, player *_session, std::multimap< int32_t/*ID*/, CaddieInfoEx >& v_element, int option = 0);
			static int						pacote072(packet& p, player *_session, UserEquip ue, int option = 0);
			static int						pacote073(packet& p, player *_session, std::multimap< int32_t/*ID*/, WarehouseItemEx >& v_element, int option = 0);
			static int						pacote089(packet& p, player *_session, uint32_t _uid, unsigned char season, uint32_t err_code = 1);
			static int						pacote095(packet& p, player *_session, unsigned short sub_tipo, int option = 0, PlayerInfo *pi = nullptr);
			static int						pacote096(packet& p, player *_session, PlayerInfo *pi);
			static int						pacote09A(packet& p, player *_session, PlayerInfo *pi);
			static int						pacote09F(packet& p, player *_session, std::vector< ServerInfo >& v_server, std::vector< channel* >& v_channel);
			static int						pacote0AA(packet& p, player *_session, std::vector< stItem >& v_item);
			static int						pacote0B2(packet& p, player *_session, std::vector< MsgOffInfo >& v_element, int option = 0);
			static int						pacote0B4(packet& p, player *_session, std::vector< TrofelEspecialInfo >& v_element, int option = 0);
			static int						pacote0D4(packet& p, player *_session, std::multimap< int32_t/*ID*/, CaddieInfoEx >& v_element);
			static int						pacote0E1(packet& p, player *_session, std::multimap< int32_t/*ID*/, MascotInfoEx >& v_element, int option = 0);
			static int						pacote0F1(packet& p, player *_session, int option = 0);
			static int						pacote0F5(packet& p, player *_session);
			static int						pacote0F6(packet& p, player *_session);
			static int						pacote0FC(packet& p, player *_session, std::vector< ServerInfo >& v_si);
			static int						pacote101(packet& p, player *_session, int option = 0);
			static int						pacote102(packet& p, player *_session, PlayerInfo *pi);
			static int						pacote10E(packet& p, player *_session, Last5PlayersGame& l5pg);
			static int						pacote11F(packet& p, player *_session, PlayerInfo *pi, short tipo);
			static int						pacote12E(packet& p, player *_session, WarehouseItemEx *wi, int state, int option = 0);
			static int						pacote131(packet& p, int option = 1);
			static int						pacote135(packet& p, player *_session);
			static int						pacote136(packet& p, player *_session);
			static int						pacote137(packet& p, player *_session, std::vector< CardEquipInfoEx >& v_element);
			static int						pacote138(packet& p, player *_session, std::vector< CardInfo >& v_element, int option = 0);
			static int						pacote13F(packet& p, player *_session, int option = 0);
			static int						pacote144(packet& p, player *_session, int option = 0);
			static int						pacote156(packet& p, player *_session, uint32_t _uid, UserEquip& _ue, unsigned char season);
			static int						pacote157(packet& p, player *_session, MemberInfoEx& _mi, unsigned char season);
			static int						pacote158(packet& p, player *_session, uint32_t _uid, UserInfo& _ui, unsigned char season);
			static int						pacote159(packet& p, player *_session, uint32_t _uid, TrofelInfo& ti, unsigned char season);
			static int						pacote15A(packet& p, player *_session, uint32_t _uid, std::vector< TrofelEspecialInfo >& v_tei, unsigned char season);
			static int						pacote15B(packet& p, player *_session, uint32_t _uid, unsigned char season);
			static int						pacote15C(packet& p, player *_session, uint32_t _uid, std::vector< MapStatistics >& v_ms, std::vector< MapStatistics >& v_msa, unsigned char season);
			static int						pacote15D(packet& p, player *_session, uint32_t _uid, GuildInfo& _gi);
			static int						pacote15E(packet& p, player *_session, uint32_t _uid, CharacterInfo& _ci);
			static int						pacote169(packet& p, player *_session, TrofelInfo& ti, int option = 0);
			static int						pacote181(packet& p, player *_session, std::vector< ItemBuffEx >& v_element, int option = 0);
			static int						pacote1A9(packet& p, player *_session, int32_t ttl_milliseconds/*time to live*/, int option = 1);
			static int						pacote1AD(packet& p, player *_session, std::string webKey, int option = 0);
			static int						pacote1B1(packet& p, player *_session);
			static int						pacote1D4(packet& p, player *_session, std::string _AuthKeyLogin, int option = 0);
			static int						pacote210(packet& p, player *_session, std::vector< MailBox >& v_element, int option = 0);
			static int						pacote211(packet& p, player *_session, std::vector< MailBox > v_element, int32_t pagina, int32_t paginas, int error = 0);
			static int						pacote212(packet& p, player *_session, EmailInfo& ei, int error = 0);
			static int						pacote214(packet& p, player *_session, int error = 0);
			static int						pacote215(packet& p, player *_session, std::vector< MailBox > v_element, int32_t pagina, int32_t paginas, int error = 0);
			static int						pacote216(packet& p, player *_session, std::vector< stItem > & v_item, int option = 0);
			static int						pacote21D(packet& p, player *_session, std::vector< CounterItemInfo >& v_element, int option = 0);
			static int						pacote21E(packet& p, player *_session, std::multimap< uint32_t, AchievementInfoEx >& v_element, int option = 0);
			static int						pacote225(packet& p, player *_session, DailyQuestInfoUser& _dq, std::vector< RemoveDailyQuestUser > _delete_quest, int option = 0);
			static int						pacote226(packet& p, player *_session, std::vector< AchievementInfoEx >& v_element, int option = 0);
			static int						pacote227(packet& p, player *_session, std::vector< AchievementInfoEx >& v_element, int option = 0);
			static int						pacote228(packet& p, player *_session, std::vector< AchievementInfoEx >& v_element, int option = 0);
			static int						pacote22C(packet& p, player *_session, int option = 0);
			static int						pacote22D(packet& p, player *_session, std::multimap< uint32_t, AchievementInfoEx >& v_element, int option = 0);
			static int						pacote248(packet& p, player *_session, AttendanceRewardInfo& ari, int option = 0);
			static int						pacote249(packet& p, player *_session, AttendanceRewardInfo& ari, int option = 0);
			static int						pacote257(packet& p, player *_session, uint32_t _uid, std::vector< TrofelEspecialInfo >& v_tegi, unsigned char season);
			static int						pacote25D(packet& p, player *_session, std::vector< TrofelEspecialInfo >& v_element, int option = 0);
			static int						pacote26D(packet& p, player *_session, uint32_t _unix_end_date);	// Passa o Unix End Date do Premium Ticket

			// Metôdos de auxílio
			static int principal(packet& p, PlayerInfo *pi, ServerInfoEx& _si);

			// Metôdos de auxílio de criação de pacotes
			static void channel_broadcast(channel& _channel, packet& p, unsigned char _debug);
			static void channel_broadcast(channel& _channel, std::vector< packet* > v_p, unsigned char _debug);

			static void lobby_broadcast(channel& _channel, packet& p, unsigned char _debug);

			static void room_broadcast(room& _room, packet& p, unsigned char _debug);
			static void room_broadcast(room& _room, std::vector< packet* > v_p, unsigned char _debug);

			static void game_broadcast(Game& _game, packet& p, unsigned char _debug);
			static void game_broadcast(Game& _game, std::vector< packet* > v_p, unsigned char _debug);

			static void vector_send(packet& _p, std::vector< session* > _v_s, unsigned char _debug);
			static void vector_send(packet& _p, std::vector< player* > _v_s, unsigned char _debug);
			static void vector_send(std::vector< packet* > _v_p, std::vector< session* > _v_s, unsigned char _debug);
			static void vector_send(std::vector< packet* > _v_p, std::vector< player* > _v_s, unsigned char _debug);

			static void session_send(packet& p, session *s, unsigned char _debug);
			static void session_send(std::vector< packet* > v_p, session *s, unsigned char _debug);
    };
}

#endif