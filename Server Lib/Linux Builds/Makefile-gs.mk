# DEFAULT GOAL TARGET
.DEFAULT_GOAL := all

# Compiler and linker
CXX := g++

# Debug
DEBUG_CXXFLAGS := -Og -D _DEBUG -D SHOW_DEBUG_PACKET -ggdb
DEBUG_OBJDIR_PREFIX := debug

# Release
RELEASE_CXXFLAGS := -O3
RELEASE_OBJDIR_PREFIX := release

CURRENT_CXXFLAGS := $(DEBUG_CXXFLAGS)
CURRENT_OBJDIR_PREFIX := $(DEBUG_OBJDIR_PREFIX)
BUILD_TYPE_INFO := ****** Build Type=Debug ******

ifdef __build_type
	ifeq ($(__build_type), release)
		CURRENT_CXXFLAGS = $(RELEASE_CXXFLAGS)
		CURRENT_OBJDIR_PREFIX = $(RELEASE_OBJDIR_PREFIX)
		BUILD_TYPE_INFO = ****** Build Type=Release ******
	endif
endif

# -fpermissive -Wall -Wextra
CXXFLAGS := $(CURRENT_CXXFLAGS) -fpermissive -fsigned-char -std=c++20 -fvisibility=hidden `pkg-config --cflags glib-2.0`

# /usr/lib/x86_64-linux-gnu/libglib-2.0.so.0.6400.6 /usr/lib/libmsodbcsql-17.so
CXXLINK := -Wl,--export-dynamic -pthread -lg -ldl `pkg-config --libs libzip` `pkg-config --libs mysqlclient` -lodbc -lcrypto `pkg-config --libs glib-2.0`

# set library static owner to check modify and recompile program
LIBSTATIC := GGSrvLib26-1/GGSrvLib26-1.a

# Program name
PROGRAM := game

# Directory Output program App
DIRSAVEPRROGRAM := ./Game\ Server

# The build folder, for all generated output. This should normally be included
# in a .gitignore rule
BUILD_FOLDER := output-gs-$(CURRENT_OBJDIR_PREFIX)

# my addiction
CFILES := \
	../Game|Server/Game|Server/Game|Server.cpp \
	../Game|Server/Game|Server/game_server.cpp \
	../Projeto|IOCP/UTIL/exception.cpp \
	../Projeto|IOCP/UTIL/message_pool.cpp \
	../Projeto|IOCP/UTIL/message.cpp \
	../Projeto|IOCP/TIMER/queue_timer.cpp \
	../Projeto|IOCP/UTIL/event.cpp \
	../Projeto|IOCP/THREAD|POOL/thread.cpp \
	../Projeto|IOCP/UTIL/WinPort.cpp \
	../Projeto|IOCP/UTIL/reader_ini.cpp \
	../Projeto|IOCP/UTIL/util_time.cpp \
	../Projeto|IOCP/THREAD|POOL/job_pool.cpp \
	../Projeto|IOCP/THREAD|POOL/job.cpp \
	../Projeto|IOCP/TIMER/timer.cpp \
	../Projeto|IOCP/TIMER/timer_manager.cpp \
	../Projeto|IOCP/UTIL/hex_util.cpp \
	../Projeto|IOCP/THREAD|POOL/threadpool.cpp \
	../Projeto|IOCP/IOCP/epoll.cpp \
	../Projeto|IOCP/UTIL/buffer.cpp \
	../Projeto|IOCP/SOCKET/session.cpp \
	../Projeto|IOCP/PACKET/packet.cpp \
	../Projeto|IOCP/COMPRESS/compress.cpp \
	../Projeto|IOCP/COMPRESS/minilzo.c \
	../Projeto|IOCP/CRYPT/crypt.cpp \
	../Projeto|IOCP/DATABASE/normal_manager_db.cpp \
	../Projeto|IOCP/DATABASE/normal_db.cpp \
	../Projeto|IOCP/DATABASE/response.cpp \
	../Projeto|IOCP/DATABASE/result_set.cpp \
	../Projeto|IOCP/SOCKET/socketserver.cpp \
	../Projeto|IOCP/SOCKET/socket.cpp \
	../Projeto|IOCP/PANGYA_DB/pangya_db.cpp \
	../Projeto|IOCP/DATABASE/exec_query.cpp \
	../Projeto|IOCP/PACKET/packet_func.cpp \
	../Projeto|IOCP/UTIL/func_arr.cpp \
	../Projeto|IOCP/DATABASE/database.cpp \
	../Projeto|IOCP/DATABASE/mssql.cpp \
	../Projeto|IOCP/DATABASE/postgresql.cpp \
	../Projeto|IOCP/DATABASE/mysql_db.cpp \
	../Projeto|IOCP/Server/server.cpp \
	../Projeto|IOCP/THREAD|POOL/threadpl_server.cpp \
	../Projeto|IOCP/UTIL/iff.cpp \
	../Projeto|IOCP/SOCKET/session_manager.cpp \
	../Projeto|IOCP/UNIT/unit_connect.cpp \
	../Projeto|IOCP/UNIT/unit_auth_server_connect.cpp \
	../Projeto|IOCP/Smart|Calculator/Smart|Calculator.cpp \
	../Game|Server/SESSION/player.cpp \
	../Game|Server/SESSION/player_manager.cpp \
	../Game|Server/PACKET/packet_func_sv.cpp \
	../Game|Server/UTIL/broadcast_list.cpp \
	../Game|Server/GAME/login_manager.cpp \
	../Game|Server/PANGYA_DB/cmd_achievement_info.cpp \
	../Game|Server/PANGYA_DB/cmd_add_ball.cpp \
	../Game|Server/PANGYA_DB/cmd_add_caddie.cpp \
	../Game|Server/PANGYA_DB/cmd_add_card.cpp \
	../Game|Server/PANGYA_DB/cmd_add_character_hair_style.cpp \
	../Game|Server/PANGYA_DB/cmd_add_clubset.cpp \
	../Game|Server/PANGYA_DB/cmd_add_counter_item.cpp \
	../Game|Server/PANGYA_DB/cmd_add_daily_quest.cpp \
	../Game|Server/PANGYA_DB/cmd_add_dolfini_locker_item.cpp \
	../Game|Server/PANGYA_DB/cmd_add_furniture.cpp \
	../Game|Server/PANGYA_DB/cmd_add_item.cpp \
	../Game|Server/PANGYA_DB/cmd_add_login_reward_player.cpp \
	../Game|Server/PANGYA_DB/cmd_add_mascot.cpp \
	../Game|Server/PANGYA_DB/cmd_add_msg_mail.cpp \
	../Game|Server/PANGYA_DB/cmd_add_part.cpp \
	../Game|Server/PANGYA_DB/cmd_add_skin.cpp \
	../Game|Server/PANGYA_DB/cmd_add_trofel_especial.cpp \
	../Game|Server/PANGYA_DB/cmd_approach_missions.cpp \
	../Game|Server/PANGYA_DB/cmd_attendance_reward_info.cpp \
	../Game|Server/PANGYA_DB/cmd_attendance_reward_item_info.cpp \
	../Game|Server/PANGYA_DB/cmd_bot_gm_event_info.cpp \
	../Game|Server/PANGYA_DB/cmd_box_info.cpp \
	../Game|Server/PANGYA_DB/cmd_caddie_info.cpp \
	../Game|Server/PANGYA_DB/cmd_card_equip_info.cpp \
	../Game|Server/PANGYA_DB/cmd_card_info.cpp \
	../Game|Server/PANGYA_DB/cmd_card_pack.cpp \
	../Game|Server/PANGYA_DB/cmd_character_info.cpp \
	../Game|Server/PANGYA_DB/cmd_check_achievement.cpp \
	../Game|Server/PANGYA_DB/cmd_coin_cube_info.cpp \
	../Game|Server/PANGYA_DB/cmd_coin_cube_location_info.cpp \
	../Game|Server/PANGYA_DB/cmd_comet_refill_info.cpp \
	../Game|Server/PANGYA_DB/cmd_cookie.cpp \
	../Game|Server/PANGYA_DB/cmd_coupon_gacha.cpp \
	../Game|Server/PANGYA_DB/cmd_create_achievement.cpp \
	../Game|Server/PANGYA_DB/cmd_create_quest.cpp \
	../Game|Server/PANGYA_DB/cmd_daily_quest_info_user.cpp \
	../Game|Server/PANGYA_DB/cmd_daily_quest_info.cpp \
	../Game|Server/PANGYA_DB/cmd_delete_achievement.cpp \
	../Game|Server/PANGYA_DB/cmd_delete_ball.cpp \
	../Game|Server/PANGYA_DB/cmd_delete_caddie.cpp \
	../Game|Server/PANGYA_DB/cmd_delete_card.cpp \
	../Game|Server/PANGYA_DB/cmd_delete_counter_item.cpp \
	../Game|Server/PANGYA_DB/cmd_delete_daily_quest.cpp \
	../Game|Server/PANGYA_DB/cmd_delete_dolfini_locker_item.cpp \
	../Game|Server/PANGYA_DB/cmd_delete_email.cpp \
	../Game|Server/PANGYA_DB/cmd_delete_furniture.cpp \
	../Game|Server/PANGYA_DB/cmd_delete_item.cpp \
	../Game|Server/PANGYA_DB/cmd_delete_mascot.cpp \
	../Game|Server/PANGYA_DB/cmd_delete_quest.cpp \
	../Game|Server/PANGYA_DB/cmd_delete_rental.cpp \
	../Game|Server/PANGYA_DB/cmd_dolfini_locker_info.cpp \
	../Game|Server/PANGYA_DB/cmd_drop_course_config.cpp \
	../Game|Server/PANGYA_DB/cmd_drop_course_info.cpp \
	../Game|Server/PANGYA_DB/cmd_email_info.cpp \
	../Game|Server/PANGYA_DB/cmd_email_info2.cpp \
	../Game|Server/PANGYA_DB/cmd_equip_card.cpp \
	../Game|Server/PANGYA_DB/cmd_extend_rental.cpp \
	../Game|Server/PANGYA_DB/cmd_find_caddie.cpp \
	../Game|Server/PANGYA_DB/cmd_find_card.cpp \
	../Game|Server/PANGYA_DB/cmd_find_character.cpp \
	../Game|Server/PANGYA_DB/cmd_find_dolfini_locker_item.cpp \
	../Game|Server/PANGYA_DB/cmd_find_furniture.cpp \
	../Game|Server/PANGYA_DB/cmd_find_mail_box_item.cpp \
	../Game|Server/PANGYA_DB/cmd_find_mascot.cpp \
	../Game|Server/PANGYA_DB/cmd_find_trofel_especial.cpp \
	../Game|Server/PANGYA_DB/cmd_find_ucc.cpp \
	../Game|Server/PANGYA_DB/cmd_find_warehouse_item.cpp \
	../Game|Server/PANGYA_DB/cmd_friend_info.cpp \
	../Game|Server/PANGYA_DB/cmd_gera_ucc_web_key.cpp \
	../Game|Server/PANGYA_DB/cmd_gera_web_key.cpp \
	../Game|Server/PANGYA_DB/cmd_get_gift_clubset.cpp \
	../Game|Server/PANGYA_DB/cmd_get_gift_part.cpp \
	../Game|Server/PANGYA_DB/cmd_gift_clubset.cpp \
	../Game|Server/PANGYA_DB/cmd_gift_part.cpp \
	../Game|Server/PANGYA_DB/cmd_golden_time_info.cpp \
	../Game|Server/PANGYA_DB/cmd_golden_time_item.cpp \
	../Game|Server/PANGYA_DB/cmd_golden_time_round.cpp \
	../Game|Server/PANGYA_DB/cmd_grand_prix_clear.cpp \
	../Game|Server/PANGYA_DB/cmd_grand_zodiac_event_info.cpp \
	../Game|Server/PANGYA_DB/cmd_grand_zodiac_pontos.cpp \
	../Game|Server/PANGYA_DB/cmd_guild_info.cpp \
	../Game|Server/PANGYA_DB/cmd_guild_update_activity_info.cpp \
	../Game|Server/PANGYA_DB/cmd_insert_box_rare_win_log.cpp \
	../Game|Server/PANGYA_DB/cmd_insert_command.cpp \
	../Game|Server/PANGYA_DB/cmd_insert_cp_log_item.cpp \
	../Game|Server/PANGYA_DB/cmd_insert_cp_log.cpp \
	../Game|Server/PANGYA_DB/cmd_insert_grand_prix_clear.cpp \
	../Game|Server/PANGYA_DB/cmd_insert_memorial_rare_win_log.cpp \
	../Game|Server/PANGYA_DB/cmd_insert_msg_off.cpp \
	../Game|Server/PANGYA_DB/cmd_insert_papel_shop_rare_win_log.cpp \
	../Game|Server/PANGYA_DB/cmd_insert_spinning_cube_super_rare_win_broadcast.cpp \
	../Game|Server/PANGYA_DB/cmd_insert_ticker.cpp \
	../Game|Server/PANGYA_DB/cmd_insert_ticket_report_data.cpp \
	../Game|Server/PANGYA_DB/cmd_insert_ticket_report.cpp \
	../Game|Server/PANGYA_DB/cmd_item_buff_info.cpp \
	../Game|Server/PANGYA_DB/cmd_item_left_from_email.cpp \
	../Game|Server/PANGYA_DB/cmd_last_player_game_info.cpp \
	../Game|Server/PANGYA_DB/cmd_legacy_tiki_shop_info.cpp \
	../Game|Server/PANGYA_DB/cmd_login_reward_info.cpp \
	../Game|Server/PANGYA_DB/cmd_login_reward_player_info.cpp \
	../Game|Server/PANGYA_DB/cmd_mail_box_info.cpp \
	../Game|Server/PANGYA_DB/cmd_mail_box_info2.cpp \
	../Game|Server/PANGYA_DB/cmd_map_statistics.cpp \
	../Game|Server/PANGYA_DB/cmd_mascot_info.cpp \
	../Game|Server/PANGYA_DB/cmd_member_info.cpp \
	../Game|Server/PANGYA_DB/cmd_memorial_level_info.cpp \
	../Game|Server/PANGYA_DB/cmd_memorial_normal_item_info.cpp \
	../Game|Server/PANGYA_DB/cmd_msg_off_info.cpp \
	../Game|Server/PANGYA_DB/cmd_my_room_config.cpp \
	../Game|Server/PANGYA_DB/cmd_my_room_item.cpp \
	../Game|Server/PANGYA_DB/cmd_old_daily_quest_info.cpp \
	../Game|Server/PANGYA_DB/cmd_pang.cpp \
	../Game|Server/PANGYA_DB/cmd_papel_shop_config.cpp \
	../Game|Server/PANGYA_DB/cmd_papel_shop_coupon.cpp \
	../Game|Server/PANGYA_DB/cmd_papel_shop_item.cpp \
	../Game|Server/PANGYA_DB/cmd_pay_caddie_holy_day.cpp \
	../Game|Server/PANGYA_DB/cmd_personal_shop_log.cpp \
	../Game|Server/PANGYA_DB/cmd_player_info.cpp \
	../Game|Server/PANGYA_DB/cmd_premium_ticket_info.cpp \
	../Game|Server/PANGYA_DB/cmd_put_item_mail_box.cpp \
	../Game|Server/PANGYA_DB/cmd_register_guild_match.cpp \
	../Game|Server/PANGYA_DB/cmd_register_logon_server.cpp \
	../Game|Server/PANGYA_DB/cmd_remove_equiped_card.cpp \
	../Game|Server/PANGYA_DB/cmd_set_notice_caddie_holy_day.cpp \
	../Game|Server/PANGYA_DB/cmd_ticket_report_dados_info.cpp \
	../Game|Server/PANGYA_DB/cmd_transfer_part.cpp \
	../Game|Server/PANGYA_DB/cmd_treasure_hunter_info.cpp \
	../Game|Server/PANGYA_DB/cmd_treasure_hunter_item.cpp \
	../Game|Server/PANGYA_DB/cmd_trofel_info.cpp \
	../Game|Server/PANGYA_DB/cmd_trophy_special.cpp \
	../Game|Server/PANGYA_DB/cmd_tuto_event_clear.cpp \
	../Game|Server/PANGYA_DB/cmd_tutorial_info.cpp \
	../Game|Server/PANGYA_DB/cmd_update_achievement_user.cpp \
	../Game|Server/PANGYA_DB/cmd_update_attendance_reward.cpp \
	../Game|Server/PANGYA_DB/cmd_update_ball_equiped.cpp \
	../Game|Server/PANGYA_DB/cmd_update_ball_qntd.cpp \
	../Game|Server/PANGYA_DB/cmd_update_caddie_equiped.cpp \
	../Game|Server/PANGYA_DB/cmd_update_caddie_info.cpp \
	../Game|Server/PANGYA_DB/cmd_update_caddie_item.cpp \
	../Game|Server/PANGYA_DB/cmd_update_card_qntd.cpp \
	../Game|Server/PANGYA_DB/cmd_update_card_special_time.cpp \
	../Game|Server/PANGYA_DB/cmd_update_character_all_part_equiped.cpp \
	../Game|Server/PANGYA_DB/cmd_update_character_cutin_equiped.cpp \
	../Game|Server/PANGYA_DB/cmd_update_character_mastery.cpp \
	../Game|Server/PANGYA_DB/cmd_update_character_pcl.cpp \
	../Game|Server/PANGYA_DB/cmd_update_chat_macro_user.cpp \
	../Game|Server/PANGYA_DB/cmd_update_clubset_equiped.cpp \
	../Game|Server/PANGYA_DB/cmd_update_clubset_stats.cpp \
	../Game|Server/PANGYA_DB/cmd_update_clubset_time.cpp \
	../Game|Server/PANGYA_DB/cmd_update_clubset_workshop.cpp \
	../Game|Server/PANGYA_DB/cmd_update_coin_cube_location.cpp \
	../Game|Server/PANGYA_DB/cmd_update_cookie.cpp \
	../Game|Server/PANGYA_DB/cmd_update_counter_item.cpp \
	../Game|Server/PANGYA_DB/cmd_update_daily_quest_user.cpp \
	../Game|Server/PANGYA_DB/cmd_update_daily_quest.cpp \
	../Game|Server/PANGYA_DB/cmd_update_dolfini_locker_mode.cpp \
	../Game|Server/PANGYA_DB/cmd_update_dolfini_locker_pang.cpp \
	../Game|Server/PANGYA_DB/cmd_update_dolfini_locker_pass.cpp \
	../Game|Server/PANGYA_DB/cmd_update_email.cpp \
	../Game|Server/PANGYA_DB/cmd_update_golden_time.cpp \
	../Game|Server/PANGYA_DB/cmd_update_grand_prix_clear.cpp \
	../Game|Server/PANGYA_DB/cmd_update_guild_member_points.cpp \
	../Game|Server/PANGYA_DB/cmd_update_guild_points.cpp \
	../Game|Server/PANGYA_DB/cmd_update_guild_update_activity.cpp \
	../Game|Server/PANGYA_DB/cmd_update_item_buff.cpp \
	../Game|Server/PANGYA_DB/cmd_update_item_qntd.cpp \
	../Game|Server/PANGYA_DB/cmd_update_item_slot.cpp \
	../Game|Server/PANGYA_DB/cmd_update_last_player_game.cpp \
	../Game|Server/PANGYA_DB/cmd_update_legacy_tiki_shop_point.cpp \
	../Game|Server/PANGYA_DB/cmd_update_level_and_exp.cpp \
	../Game|Server/PANGYA_DB/cmd_update_login_reward_player.cpp \
	../Game|Server/PANGYA_DB/cmd_update_login_reward.cpp \
	../Game|Server/PANGYA_DB/cmd_update_map_statistics.cpp \
	../Game|Server/PANGYA_DB/cmd_update_mascot_equiped.cpp \
	../Game|Server/PANGYA_DB/cmd_update_mascot_info.cpp \
	../Game|Server/PANGYA_DB/cmd_update_mascot_time.cpp \
	../Game|Server/PANGYA_DB/cmd_update_normal_trofel.cpp \
	../Game|Server/PANGYA_DB/cmd_update_pang.cpp \
	../Game|Server/PANGYA_DB/cmd_update_papel_shop_config.cpp \
	../Game|Server/PANGYA_DB/cmd_update_papel_shop_info.cpp \
	../Game|Server/PANGYA_DB/cmd_update_player_location.cpp \
	../Game|Server/PANGYA_DB/cmd_update_poster_equiped.cpp \
	../Game|Server/PANGYA_DB/cmd_update_premium_ticket_time.cpp \
	../Game|Server/PANGYA_DB/cmd_update_quest_user.cpp \
	../Game|Server/PANGYA_DB/cmd_update_skin_equiped.cpp \
	../Game|Server/PANGYA_DB/cmd_update_treasure_hunter_course_point.cpp \
	../Game|Server/PANGYA_DB/cmd_update_trofel_especial_qntd.cpp \
	../Game|Server/PANGYA_DB/cmd_update_tutorial.cpp \
	../Game|Server/PANGYA_DB/cmd_update_ucc.cpp \
	../Game|Server/PANGYA_DB/cmd_update_user_info.cpp \
	../Game|Server/PANGYA_DB/cmd_use_item_buff.cpp \
	../Game|Server/PANGYA_DB/cmd_user_equip.cpp \
	../Game|Server/PANGYA_DB/cmd_user_info.cpp \
	../Game|Server/PANGYA_DB/cmd_warehouse_item.cpp \
	../Game|Server/GAME/approach_mission_system.cpp \
	../Game|Server/GAME/approach.cpp \
	../Game|Server/GAME/attendance_reward_system.cpp \
	../Game|Server/GAME/bot_gm_event.cpp \
	../Game|Server/GAME/box_system.cpp \
	../Game|Server/GAME/card_system.cpp \
	../Game|Server/GAME/channel.cpp \
	../Game|Server/GAME/chip_in_practice.cpp \
	../Game|Server/GAME/coin_cube_location_update_system.cpp \
	../Game|Server/GAME/comet_refill_system.cpp \
	../Game|Server/GAME/course.cpp \
	../Game|Server/GAME/cube_coin_system.cpp \
	../Game|Server/GAME/drop_system.cpp \
	../Game|Server/GAME/dupla_manager.cpp \
	../Game|Server/GAME/dupla.cpp \
	../Game|Server/GAME/game.cpp \
	../Game|Server/GAME/golden_time_system.cpp \
	../Game|Server/GAME/grand_prix.cpp \
	../Game|Server/GAME/grand_zodiac_base.cpp \
	../Game|Server/GAME/grand_zodiac_event.cpp \
	../Game|Server/GAME/grand_zodiac.cpp \
	../Game|Server/GAME/guild_battle.cpp \
	../Game|Server/GAME/guild_room_manager.cpp \
	../Game|Server/GAME/guild.cpp \
	../Game|Server/GAME/hole.cpp \
	../Game|Server/GAME/item_manager.cpp \
	../Game|Server/GAME/item.cpp \
	../Game|Server/GAME/lobby.cpp \
	../Game|Server/GAME/login_reward_system.cpp \
	../Game|Server/GAME/login_task.cpp \
	../Game|Server/GAME/mail_box_manager.cpp \
	../Game|Server/GAME/match.cpp \
	../Game|Server/GAME/memorial_system.cpp \
	../Game|Server/GAME/pang_battle.cpp \
	../Game|Server/GAME/papel_shop_system.cpp \
	../Game|Server/GAME/personal_shop.cpp \
	../Game|Server/GAME/player_mail_box.cpp \
	../Game|Server/GAME/practice.cpp \
	../Game|Server/GAME/premium_system.cpp \
	../Game|Server/GAME/room_bot_gm_event.cpp \
	../Game|Server/GAME/room_grand_prix.cpp \
	../Game|Server/GAME/room_grand_zodiac_event.cpp \
	../Game|Server/GAME/room_manager.cpp \
	../Game|Server/GAME/room.cpp \
	../Game|Server/GAME/special_shuffle_course.cpp \
	../Game|Server/GAME/team.cpp \
	../Game|Server/GAME/tourney_base.cpp \
	../Game|Server/GAME/tourney.cpp \
	../Game|Server/GAME/treasure_hunter_system.cpp \
	../Game|Server/GAME/versus_base.cpp \
	../Game|Server/GAME/versus.cpp \
	../Game|Server/UTIL/ball3d.cpp \
	../Game|Server/UTIL/block_exec_one_per_time.cpp \
	../Game|Server/UTIL/block_memory_manager.cpp \
	../Game|Server/UTIL/club_info3d.cpp \
	../Game|Server/UTIL/club3d.cpp \
	../Game|Server/UTIL/extra_power.cpp \
	../Game|Server/UTIL/lottery.cpp \
	../Game|Server/UTIL/map.cpp \
	../Game|Server/UTIL/matrix3d.cpp \
	../Game|Server/UTIL/mgr_achievement.cpp \
	../Game|Server/UTIL/mgr_daily_quest.cpp \
	../Game|Server/UTIL/quad_tree3d.cpp \
	../Game|Server/UTIL/sys_achievement.cpp \
	../Game|Server/UTIL/vector3d.cpp \
	../Game|Server/TYPE/_3d_type.cpp \
	../Game|Server/UNIT/unit_gg_auth_server_connect.cpp \
	../Game|Server/CSAuth/GGAuth.cpp \
	../Game|Server/TYPE/player_info.cpp \
	../Game|Server/TYPE/gm_info.cpp \
	../Projeto|IOCP/PANGYA_DB/cmd_register_server.cpp \
	../Projeto|IOCP/PANGYA_DB/cmd_server_list.cpp \
	../Projeto|IOCP/PANGYA_DB/cmd_list_ip_ban.cpp \
	../Projeto|IOCP/PANGYA_DB/cmd_list_mac_ban.cpp \
	../Projeto|IOCP/PANGYA_DB/cmd_new_auth_server_key.cpp \
	../Projeto|IOCP/PANGYA_DB/cmd_insert_block_ip.cpp \
	../Projeto|IOCP/PANGYA_DB/cmd_insert_block_mac.cpp \
	../Projeto|IOCP/PANGYA_DB/cmd_add_character.cpp \
	../Projeto|IOCP/PANGYA_DB/cmd_register_logon.cpp \
	../Projeto|IOCP/PANGYA_DB/cmd_auth_key_login_info.cpp \
	../Projeto|IOCP/PANGYA_DB/cmd_auth_key_game.cpp \
	../Projeto|IOCP/PANGYA_DB/cmd_update_auth_key_login.cpp \
	../Projeto|IOCP/PANGYA_DB/cmd_rate_config_info.cpp \
	../Projeto|IOCP/PANGYA_DB/cmd_update_rate_config_info.cpp \
	../Projeto|IOCP/PANGYA_DB/cmd_update_character_equiped.cpp \
	../Projeto|IOCP/PANGYA_DB/cmd_verify_nick.cpp \
	../Projeto|IOCP/PANGYA_DB/cmd_chat_macro_user.cpp \
	../Projeto|IOCP/PANGYA_DB/cmd_add_item_base.cpp \
	../Projeto|IOCP/UTIL/random_gen.cpp \
	../Projeto|IOCP/UTIL/md5.cpp \
	../Game|Server/GAME/personal_shop_manager.cpp

target = $(BUILD_FOLDER)/$(subst |,\ ,$(filter %.o,$(patsubst %.cpp,%.o,${1}) $(patsubst %.c,%.o,${1})))
target_dep = $(BUILD_FOLDER)/$(subst |,\ ,$(filter %.o.d,$(patsubst %.cpp,%.o.d,${1}) $(patsubst %.c,%.o.d,${1})))
obj.cpp :=
obj.c :=
define obj
 $(call target,$(notdir ${1})): $(subst |,\ ,${1}) | ${BUILD_FOLDER}
 obj$(suffix ${1}) += $(call target,$(notdir ${1}))
endef

define SOURCES
 $(foreach src,${1},$(eval $(call obj,${src})))
endef

$(eval $(call SOURCES,${CFILES}))

define obj_link
	$(foreach src,${CFILES},$(call target,$(notdir ${src})))
endef

define dep_link
	$(foreach src,${CFILES},$(call target_dep,$(notdir ${src})))
endef

# scape white space
space := $(subst ,, )

define scape_WS
 $(subst $(space),\ ,${1})
endef

# Default all rule will build the 'example' target, which here is an executable
.PHONY:
all: $(BUILD_FOLDER)/$(PROGRAM)

# Show Info Build Type
$(info $(BUILD_TYPE_INFO))

# Generate a list of .o files from the .c files. Prefix them with the build
# folder to output the files there
OBJ_FILES := $(call obj_link)
#$(addprefix $(BUILD_FOLDER)/,$(SRC_FILES:.cpp=.o))

# Generate a list of depfiles, used to track includes. The file name is the same
# as the object files with the .d extension added
DEP_FILES := $(call dep_link)
#$(addsuffix .d,$(OBJ_FILES))

# Flags to generate the .d dependency-tracking files when we compile.  It's
# named the same as the target file with the .d extension
DEPFLAGS = -MMD -MP -MF $(call scape_WS,$@).d -MT '$(strip $(call scape_WS,$@)) $(strip $(call scape_WS,$@)).d'

# Include the dependency tracking files
-include $(DEP_FILES)

# List of include dirs. These are put into CFLAGS.
#INCLUDE_DIRS = \
#    src/

# Prefix the include dirs with '-I' when passing them to the compiler
#CFLAGS += $(addprefix -I,$(INCLUDE_DIRS))

# Set some compiler flags we need. Note that we're appending to the CFLAGS
# variable
#    -Wall \
#    -Werror 
CFLAGS += $(CXXFLAGS)

# Our project requires some linker flags: garbage collect sections, output a
# .map file
#LDFLAGS += 

# Set LDLIBS to specify linking with libm, the math library
LDLIBS += $(CXXLINK)

# The rule for compiling the SRC_FILES into OBJ_FILES
${obj.cpp} ${obj.c}: %.o :
	@echo Compiling $(CXX) $(CFLAGS) -c $(call scape_WS,$<) -o $(call scape_WS,$@)
	@$(CXX) $(CFLAGS) $(DEPFLAGS) -c $(call scape_WS,$<) -o $(call scape_WS,$@)

# Make Objects to link
OUT_OBJECTS = $(subst .a\,.a, $(subst .o\,.o,$(addsuffix \,$^)))

# The rule for building the executable "example", using OBJ_FILES as
# prerequisites. Since we're not relying on an implicit rule, we need to
# explicity list CFLAGS, LDFLAGS, LDLIBS
$(BUILD_FOLDER)/$(PROGRAM): $(OBJ_FILES) $(LIBSTATIC)
	@echo Linking $(notdir $@)
	$(CXX) $(CFLAGS) $(LDFLAGS) $(OUT_OBJECTS) $(LDLIBS) -o $@
	cp $(BUILD_FOLDER)/$(PROGRAM) $(DIRSAVEPRROGRAM)/$(PROGRAM)

# Remove debug information for a smaller executable. An embedded project might
# instead using [arm-none-eabi-]objcopy to convert the ELF file to a raw binary
# suitable to be written to an embedded device
STRIPPED_OUTPUT := $(BUILD_FOLDER)/$(PROGRAM)-stripped

$(STRIPPED_OUTPUT): $(BUILD_FOLDER)/$(PROGRAM)
	@echo Stripping $(notdir $@)
	objcopy --strip-debug $^ $@
	cp $@ $(DIRSAVEPRROGRAM)/$(PROGRAM)

# make build folder
$(BUILD_FOLDER):
	@mkdir -p $@

# Since all our generated output is placed into the build folder, our clean rule
# is simple. Prefix the recipe line with '-' to not error if the build folder
# doesn't exist (the -f flag for rm also has this effect)
.PHONY: clean
clean:
	- find $(BUILD_FOLDER)/ -name "*.[od]" -exec rm -rf {} \;
