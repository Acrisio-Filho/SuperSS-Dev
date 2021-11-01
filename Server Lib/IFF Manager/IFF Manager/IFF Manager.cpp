// IFF Manager.cpp : Este arquivo contém a função 'main'. A execução do programa começa e termina ali.
//

#pragma pack(1)

#include "pch.h"
#include <iostream>
#include "../../Projeto IOCP/UTIL/message_pool.h"
#include "../UTIL/iff_writer.hpp"

#include "../../Projeto IOCP/TYPE/set_se_exception_with_mini_dump.h"

#include <DbgHelp.h>

#pragma comment(lib, "DbgHelp.lib")

using namespace stdA;

int main(int argc, char* argv[]) {

	// SEH TRANLATER CATCH VIOLATION EXCEPTION
	STDA_SET_SE_EXCEPTION;

	// Init Message_Pool Logs
	_smp::message_pool::getInstance().reload_log_files();

	struct ItemPrice {
		void clear() {
			memset(this, 0, sizeof(ItemPrice));
		};
		unsigned long _typeid;
		unsigned short price[5];
	};

	std::map< unsigned long, IFF::Base > all;
	std::map< unsigned long, ItemPrice > all_price;

	try {

		iff_writer iff;

		std::cout	<< "1. Gera Lista itemBase Ativo no shop.\n"
					<< "2. Ler Lista e alterar no iff.\n"
					<< "3. Tirar todas as data limite dos SetItem no shop.\n"
					<< "4. Gera Lista de preco dos itens de tempo no shop.\n"
					<< "5. Ler Lista de preco e alterar no iff.\n"
					<< "6. Update Seq CadieMagicBox.\n"
					<< "7. Make Table List Donate Item.\n"
					<< "8. Show All Parts 10 curva ou mais.\n"
					<< "9. Lucia Attendance System.\n"
					<< "10. Show all Card Personal shop saleble.\n"
					<< "11. Show All Tiki Shop Itens TP more or equal 15.\n"
					<< "12. Name itens gacha.\n"
					<< "13. Habilita All Cutin Fate no shop.\n"
					<< "14. Papel Shop Rare Number 28.\n"
			;

		int escolha = 0;

		std::cin >> escolha;

		if (escolha == 1) {

			// Gera Lista de Item Base que estão ativos nos shop
			for (auto& el : iff.getBall())
				if (el.second.shop.flag_shop.uFlagShop.stFlagShop.is_saleable)
					all.insert(std::make_pair(el.second._typeid, el.second));

			for (auto& el : iff.getCaddie())
				if (el.second.shop.flag_shop.uFlagShop.stFlagShop.is_saleable)
					all.insert(std::make_pair(el.second._typeid, el.second));

			for (auto& el : iff.getCaddieItem())
				if (el.second.shop.flag_shop.uFlagShop.stFlagShop.is_saleable)
					all.insert(std::make_pair(el.second._typeid, el.second));

			for (auto& el : iff.getCard())
				if (el.second.shop.flag_shop.uFlagShop.stFlagShop.is_saleable)
					all.insert(std::make_pair(el.second._typeid, el.second));

			for (auto& el : iff.getCharacter())
				if (el.second.shop.flag_shop.uFlagShop.stFlagShop.is_saleable)
					all.insert(std::make_pair(el.second._typeid, el.second));

			for (auto& el : iff.getClubSet())
				if (el.second.shop.flag_shop.uFlagShop.stFlagShop.is_saleable)
					all.insert(std::make_pair(el.second._typeid, el.second));

			for (auto& el : iff.getHairStyle())
				if (el.second.shop.flag_shop.uFlagShop.stFlagShop.is_saleable)
					all.insert(std::make_pair(el.second._typeid, el.second));

			for (auto& el : iff.getItem())
				if (el.second.shop.flag_shop.uFlagShop.stFlagShop.is_saleable)
					all.insert(std::make_pair(el.second._typeid, el.second));

			for (auto& el : iff.getMascot())
				if (el.second.shop.flag_shop.uFlagShop.stFlagShop.is_saleable)
					all.insert(std::make_pair(el.second._typeid, el.second));

			for (auto& el : iff.getPart())
				if (el.second.shop.flag_shop.uFlagShop.stFlagShop.is_saleable)
					all.insert(std::make_pair(el.second._typeid, el.second));

			for (auto& el : iff.getSetItem())
				if (el.second.shop.flag_shop.uFlagShop.stFlagShop.is_saleable)
					all.insert(std::make_pair(el.second._typeid, el.second));

			for (auto& el : iff.getSkin())
				if (el.second.shop.flag_shop.uFlagShop.stFlagShop.is_saleable)
					all.insert(std::make_pair(el.second._typeid, el.second));

			IFF::Head head{ 0 };

			head.version = IFF_VERSION;
			head.count_element = (unsigned short)all.size();
			
			std::ofstream out("data.raw", std::ofstream::binary);

			// Verifica se abriu o arquivo, para salvar os dados
			if (out.good()) {

				out.write((const char*)&head, sizeof(IFF::Head));

				for (auto& el : all)
					out.write((const char*)&el.second, sizeof(IFF::Base));

				// Fecha o arquivo que terminou de salvar
				out.close();
			}

		}else if (escolha == 2) {

			IFF::Head head{ 0 };
			IFF::Base base{ 0 };

			// Ler lista e modifica no iff
			std::ifstream in("data.raw", std::ifstream::binary);

			// Verifica se abriu o arquivo, para poder recuperar os dados nele
			if (in.good()) {

				in.read((char*)&head, sizeof(IFF::Head));

				if (head.version == IFF_VERSION && head.count_element > 0) {

					for (auto i = 0ul; i < head.count_element; ++i) {

						in.read((char*)&base, sizeof(IFF::Base));

						all.insert(std::make_pair(base._typeid, base));
					}

					if (head.count_element != (unsigned short)all.size())
						_smp::message_pool::getInstance().push(new message("[Main][Error][WARNIG] numero de elementos lidos[" + std::to_string(all.size()) 
								+ "] no data.raw e diferente do numero do cabecario[" + std::to_string(head.count_element) + "].", CL_FILE_LOG_AND_CONSOLE));
				}

				// Fecha o arquivo que terminou de recuperar os dados dele
				in.close();
			}

			if (!all.empty()) {

				IFF::Base *common = nullptr;

				for (auto& el : all) {
					
					if ((common = iff.findCommomItem(el.second._typeid)) != nullptr) {

						// Flag Shop
						common->shop.flag_shop.uFlagShop.us_flag_shop = el.second.shop.flag_shop.uFlagShop.us_flag_shop;
						
						// Desconto Shop
						common->shop.desconto = el.second.shop.desconto;
						
						// Preço Shop
						common->shop.price = el.second.shop.price;

					}else
						_smp::message_pool::getInstance().push(new message("[Main][WARNING] nao encontrou esse item no IFF[TYPEID=" + std::to_string(el.second._typeid) 
								+ "(0x" + hex_util::ltoaToHex(el.second._typeid) + ")]", CL_FILE_LOG_AND_CONSOLE));
				}

				// Save todas as modificações no iff de volta
				iff.saveAll();
			}
		
		}else if (escolha == 3) {

			// Tirar todas as datas dos SetItem no shop
			for (auto& el : iff.getSetItem())
				if (el.second.shop.flag_shop.uFlagShop.stFlagShop.is_saleable && iff.getItemSubGroupIdentify21(el.second._typeid) == 1/*Set Character Shop*/)
					el.second.date.clear();

			// Salva
			iff.saveAll();
		
		}else if (escolha == 4) {
		
			ItemPrice item_price{ 0 };

			// Gera Lista de preços de Itens de tempo que estão ativos no shop
			for (auto& el : iff.getCaddieItem()) {
				
				if (el.second.shop.flag_shop.uFlagShop.stFlagShop.is_saleable) {

					item_price.clear();

					item_price._typeid = el.second._typeid;
					item_price.price[0] = el.second.price[0];
					item_price.price[1] = el.second.price[1];
					item_price.price[2] = el.second.price[2];
					item_price.price[3] = el.second.price[3];

					all_price.insert(std::make_pair(el.second._typeid, item_price));
				}
			}

			for (auto& el : iff.getItem()) {

				// Item por tempo ativo no shop
				if (el.second.shop.flag_shop.uFlagShop.stFlagShop.is_saleable 
						&& (el.second.shop.flag_shop.time_shop.active || el.second.shop.flag_shop.time_shop.dia > 0)) {

					item_price.clear();

					item_price._typeid = el.second._typeid;
					item_price.price[0] = el.second.c[0];
					item_price.price[1] = el.second.c[1];
					item_price.price[2] = el.second.c[2];
					item_price.price[3] = el.second.c[3];
					item_price.price[4] = el.second.c[4];

					all_price.insert(std::make_pair(el.second._typeid, item_price));
				}
			}

			for (auto& el : iff.getMascot()) {

				if (el.second.shop.flag_shop.uFlagShop.stFlagShop.is_saleable) {

					item_price.clear();

					item_price._typeid = el.second._typeid;
					item_price.price[0] = el.second.price[0];
					item_price.price[1] = el.second.price[1];
					item_price.price[2] = el.second.price[2];
					item_price.price[3] = el.second.price[3];
					item_price.price[4] = el.second.price[4];

					all_price.insert(std::make_pair(el.second._typeid, item_price));
				}
			}

			for (auto& el : iff.getSkin()) {

				if (el.second.shop.flag_shop.uFlagShop.stFlagShop.is_saleable) {

					item_price.clear();

					item_price._typeid = el.second._typeid;
					item_price.price[0] = el.second.price[0];
					item_price.price[1] = el.second.price[1];
					item_price.price[2] = el.second.price[2];
					item_price.price[3] = el.second.price[3];
					item_price.price[4] = el.second.price[4];

					all_price.insert(std::make_pair(el.second._typeid, item_price));
				}
			}

			IFF::Head head{ 0 };

			head.version = IFF_VERSION;
			head.count_element = (unsigned short)all_price.size();

			std::ofstream out("data_price.raw", std::ofstream::binary);

			// Verifica se abriu o arquivo, para salvar os dados
			if (out.good()) {

				out.write((const char*)&head, sizeof(IFF::Head));

				for (auto& el : all_price)
					out.write((const char*)&el.second, sizeof(ItemPrice));

				// Fecha o arquivo que terminou de salvar
				out.close();
			}

		}else if (escolha == 5) {

			IFF::Head head{ 0 };
			ItemPrice item_price{ 0 };

			// Ler lista e modifica no iff
			std::ifstream in("data_price.raw", std::ifstream::binary);

			// Verifica se abriu o arquivo, para poder recuperar os dados nele
			if (in.good()) {

				in.read((char*)&head, sizeof(IFF::Head));

				if (head.version == IFF_VERSION && head.count_element > 0) {

					for (auto i = 0ul; i < head.count_element; ++i) {

						in.read((char*)&item_price, sizeof(ItemPrice));

						all_price.insert(std::make_pair(item_price._typeid, item_price));
					}

					if (head.count_element != (unsigned short)all_price.size())
						_smp::message_pool::getInstance().push(new message("[Main][Error][WARNIG] numero de elementos lidos[" + std::to_string(all_price.size()) 
								+ "] no data_price.raw e diferente do numero do cabecario[" + std::to_string(head.count_element) + "].", CL_FILE_LOG_AND_CONSOLE));
				}

				// Fecha o arquivo que terminou de recuperar os dados dele
				in.close();
			}

			if (!all_price.empty()) {

				IFF::Base *common = nullptr;

				for (auto& el : all_price) {
					
					switch (iff.getItemGroupIdentify(el.second._typeid)) {
					case iff::CAD_ITEM:
					{
						auto cadItem = iff.findCaddieItem(el.second._typeid);

						if (cadItem != nullptr) {
							
							cadItem->price[0] = el.second.price[0];
							cadItem->price[1] = el.second.price[1];
							cadItem->price[2] = el.second.price[2];
							cadItem->price[3] = el.second.price[3];

						}else
							_smp::message_pool::getInstance().push(new message("[Main][WARNING] nao encontrou esse CaddieItem no IFF[TYPEID=" + std::to_string(el.second._typeid)
									+ "(0x" + hex_util::ltoaToHex(el.second._typeid) + ")]", CL_FILE_LOG_AND_CONSOLE));

						break;
					}
					case iff::ITEM:
					{
						auto item = iff.findItem(el.second._typeid);

						if (item != nullptr) {
							
							item->c[0] = el.second.price[0];
							item->c[1] = el.second.price[1];
							item->c[2] = el.second.price[2];
							item->c[3] = el.second.price[3];
							item->c[4] = el.second.price[4];

						}else
							_smp::message_pool::getInstance().push(new message("[Main][WARNING] nao encontrou esse Item no IFF[TYPEID=" + std::to_string(el.second._typeid)
									+ "(0x" + hex_util::ltoaToHex(el.second._typeid) + ")]", CL_FILE_LOG_AND_CONSOLE));

						break;
					}
					case iff::MASCOT:
					{
						auto mascot = iff.findMascot(el.second._typeid);

						if (mascot != nullptr) {
							
							mascot->price[0] = (unsigned char)el.second.price[0];
							mascot->price[1] = (unsigned char)el.second.price[1];
							mascot->price[2] = (unsigned char)el.second.price[2];
							mascot->price[3] = (unsigned char)el.second.price[3];
							mascot->price[4] = (unsigned char)el.second.price[4];

						}else
							_smp::message_pool::getInstance().push(new message("[Main][WARNING] nao encontrou esse Mascot no IFF[TYPEID=" + std::to_string(el.second._typeid)
									+ "(0x" + hex_util::ltoaToHex(el.second._typeid) + ")]", CL_FILE_LOG_AND_CONSOLE));

						break;
					}
					case iff::SKIN:
					{
						auto skin = iff.findSkin(el.second._typeid);

						if (skin != nullptr) {
							
							skin->price[0] = el.second.price[0];
							skin->price[1] = el.second.price[1];
							skin->price[2] = el.second.price[2];
							skin->price[3] = el.second.price[3];
							skin->price[4] = el.second.price[4];

						}else
							_smp::message_pool::getInstance().push(new message("[Main][WARNING] nao encontrou esse Skin no IFF[TYPEID=" + std::to_string(el.second._typeid)
									+ "(0x" + hex_util::ltoaToHex(el.second._typeid) + ")]", CL_FILE_LOG_AND_CONSOLE));

						break;
					}
					default:
						_smp::message_pool::getInstance().push(new message("[Main][WARNING] GROUP ID do item nao eh de item de tempo IFF[TYPEID=" + std::to_string(el.second._typeid)
								+ "(0x" + hex_util::ltoaToHex(el.second._typeid) + ")]", CL_FILE_LOG_AND_CONSOLE));
						break;
					}
				}

				// Save todas as modificações no iff de volta
				iff.saveAll();
			}
		
		}else if (escolha == 6) {
			
			// Update Seq Cadie Magic Box
			unsigned long seq = 0ul;

			for (auto& el : iff.getCadieMagicBox())
				el.seq = ++seq;

			// Salva as modificações no CaddieMagicBox.iff e salva o .iff
			iff.saveCadieMagicBox();
		
		}else if (escolha == 7) {

			std::string table = "<h1>Itens Diversos</h1><table><tr><th>ID</th><th style=\"display: none\"></th><th>Nome</th><th>Image</th><th>Description</th></tr>";

			IFF::Desc *desc = nullptr;
			IFF::Base *common = nullptr;
			
			unsigned long id = 0ul;

			for (auto& el : iff.getMemorialShopRareItem()) {

				if ((common = iff.findCommomItem(el._typeid)) != nullptr) {

					table += "<tr>";

					table += "<td>" + std::to_string(++id) + "</td>";
					table += "<td  style=\"display: none\">" + std::to_string(el._typeid) + "</td>";
					table += "<td>" + std::string(common->name) + "</td>";
					table += "<td><img src=\"img/item/" + std::string(common->icon) + ".png\"></img></td>";

					if ((desc = iff.findDesc(el._typeid)) != nullptr)
						table += "<td>" + std::string(desc->description) + "</td>";
					else
						table += "<td>No Description</td>";

					table += "</tr>";
				}
			}

			table += "</table>";

			table += "<h1>Card</h1><table><tr><th>ID</th><th style=\"display: none\"></th><th>Nome</th><th>Volume</th><th>Image</th><th>Description</th></tr>";

			id = 0ul;

			IFF::Card::CARD_SUB_TYPE type;

			for (auto& el : iff.getCard()) {

				type = IFF::Card::CARD_SUB_TYPE(iff.getItemSubGroupIdentify22(el.second._typeid));

				if (IFF::Card::T_CADDIE == type || IFF::Card::T_CHARACTER == type || IFF::Card::T_SPECIAL == type || IFF::Card::T_NPC == type) {

					table += "<tr>";

					table += "<td>" + std::to_string(++id) + "</td>";
					table += "<td  style=\"display: none\">" + std::to_string(el.second._typeid) + "</td>";
					table += "<td>" + std::string(el.second.name) + "</td>";
					table += "<td>" + std::to_string(el.second.volume) + "</td>";
					table += "<td><img src=\"img/item/" + std::string(el.second.icon) + ".png\"></img></td>";

					if ((desc = iff.findDesc(el.second._typeid)) != nullptr)
						table += "<td>" + std::string(desc->description) + "</td>";
					else
						table += "<td>No Description</td>";

					table += "</tr>";
				}
			}

			table += "</table>";

			_smp::message_pool::getInstance().push(new message(table, CL_ONLY_FILE_LOG_TEST));

		}else if (escolha == 8) {

			auto part_curva = iff.getPart();

			std::string insert_item_10_curva_str = "";

			for (auto& el : part_curva) {

				if (el.second.c[4] >= 10 || el.second.slot[4] >= 10) {

					/*_smp::message_pool::getInstance().push(new message("Name: " + std::string(el.second.name) + "\t Typeid: " 
							+ std::to_string(el.second._typeid) + "(0x" + hex_util::ltoaToHex(el.second._typeid) 
							+ ") Character Type(" + std::to_string(iff.getItemCharIdentify(el.second._typeid)) + ")\nCurva/Slot: " 
							+ std::to_string(el.second.c[4]) + " / " + std::to_string(el.second.slot[4]) + "\r\n", CL_ONLY_FILE_LOG_TEST));*/

					insert_item_10_curva_str += "INSERT INTO [pangya].[pangya_papel_shop_item](Nome, [typeid], probabilidade, numero, tipo, active) VALUES(";

					insert_item_10_curva_str += "N'" + std::string(el.second.name) + "', ";
					insert_item_10_curva_str += std::to_string(el.second._typeid) + ", ";

					insert_item_10_curva_str += "100, 27, 2, 1)\n";
				}
			}

			_smp::message_pool::getInstance().push(new message(insert_item_10_curva_str, CL_ONLY_FILE_LOG_TEST));
		
		}else if (escolha == 9) {

			std::string table = "";

			IFF::Desc *desc = nullptr;
			IFF::Base *common = nullptr;

			unsigned long id = 0ul;

			size_t index = 0;

			auto func = [](std::string _s) {

				auto index = _s.find('\'');

				while (index != std::string::npos) {
					_s.replace(index, 1, "\\'");

					index = _s.find('\'', index + 2);
				}

				return _s;
			};

			auto getTypeName = [](unsigned long _typeid) {

				switch (sIff::getInstance().getItemGroupIdentify(_typeid)) {
				case iff::AUX_PART:
					return "Ring";
				case iff::BALL:
					return "Ball";
				case iff::CADDIE:
					return "Caddie";
				case iff::CARD:
					return "Card";
				case iff::CHARACTER:
					return "Character";
				case iff::CLUBSET:
					return "Clubset";
				case iff::ITEM:
					return "Item";
				case iff::MASCOT:
					return "Mascot";
				case iff::PART:
					return "Outfit";
				case iff::SET_ITEM:
					return "Setitem";
				}
				
				return "Unknown";
			};

			struct setiteminfo {
				unsigned long _typeid;
				std::string icon;
			};

			struct itemjson {
				unsigned long id;
				unsigned long _typeid;
				std::string name;
				std::string icon;
				std::string description;
				std::string type;
				bool is_setitem;
				std::vector< setiteminfo > v_setitem_itens;
			};

			auto getAllItemInSetItem = [&](unsigned long _typeid) {

				std::vector< setiteminfo > items;

				if (iff.getItemGroupIdentify(_typeid) == iff::SET_ITEM) {

					auto setitem = iff.findSetItem(_typeid);

					if (setitem != nullptr) {

						for (auto i = 0ul; i < (sizeof(setitem->packege.item_typeid) / sizeof(setitem->packege.item_typeid[0])); ++i) {

							if (setitem->packege.item_typeid[i] > 0ul && iff.getItemGroupIdentify(setitem->packege.item_typeid[i]) != iff::CHARACTER) {

								auto common_item = iff.findCommomItem(setitem->packege.item_typeid[i]);

								if (common_item != nullptr)
									items.push_back({ setitem->packege.item_typeid[i], common_item->icon });
							}
						}
					}
				}

				return items;
			};

			std::string type_name;

			std::map< std::string, std::vector< std::string > > map_str;
			std::map< std::string, std::vector< itemjson > > map_item_json;

			std::vector< setiteminfo > setitem_itens;

			for (auto& el : iff.getMemorialShopRareItem()) {

				if ((common = iff.findCommomItem(el._typeid)) != nullptr) {

					table = "";

					++id;

					type_name = getTypeName(el._typeid);

					table += "echo '<span name=\"" + func(std::string(common->name)) + "\" id=\"" + std::to_string(el._typeid) + "\" category=\"" + type_name + "\" class=\"lid\"><table width=\"100%\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\">\
						<tr>\
						<td align=\"center\" vAlign=\"middle\">\
						<table width=\"100%\" border=\"0\" cellspacing=\"2\" cellpadding=\"0\">\
						<tr>\
						<td style=\"width: 25px; padding-left: 5px;\">" + std::to_string(id) + "</td>\
						<td align=\"left\" width=\"20%\">\
						<img src=\"https://localhost:447/img/item/'.urlencode('" + func(std::string(common->icon)) + ".png').'\" alt=\"";
					
					if ((desc = iff.findDesc(el._typeid)) != nullptr)
						table += func(std::string(desc->description));
					else
						table += "No Description";

					table += "\" /></td>\
							  <td>" + func(std::string(common->name));

					table += "</td>\
						</tr>\
						</table>\
						</td>\
						</tr>\
						</table>\
						</span>';\n";

					// JSON
					if (iff.getItemGroupIdentify(el._typeid) == iff::SET_ITEM)
						setitem_itens = getAllItemInSetItem(el._typeid);
					else if (!setitem_itens.empty())
						setitem_itens.clear();

					map_item_json[type_name].push_back({ id, el._typeid, common->name, common->icon, (desc != nullptr ? desc->description : "No Description"), type_name, !setitem_itens.empty(), setitem_itens });

					// String
					map_str[type_name].push_back(table);

				}
			}

			// Aninal Ring
			if ((common = iff.findCommomItem(1881210882ul)) != nullptr) {

				table = "";

				++id;

				type_name = getTypeName(common->_typeid);

				table += "echo '<span name=\"" + func(std::string(common->name)) + "\" id=\"" + std::to_string(common->_typeid) + "\" category=\"" + type_name + "\" class=\"lid\"><table width=\"100%\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\">\
						<tr>\
						<td align=\"center\" vAlign=\"middle\">\
						<table width=\"100%\" border=\"0\" cellspacing=\"2\" cellpadding=\"0\">\
						<tr>\
						<td style=\"width: 25px; padding-left: 5px;\">" + std::to_string(id) + "</td>\
						<td align=\"left\" width=\"20%\">\
						<img src=\"https://localhost:447/img/item/'.urlencode('" + func(std::string(common->icon)) + ".png').'\" alt=\"";

				if ((desc = iff.findDesc(common->_typeid)) != nullptr)
					table += func(std::string(desc->description));
				else
					table += "No Description";

				table += "\" /></td>\
							  <td>" + func(std::string(common->name));

				table += "</td>\
						</tr>\
						</table>\
						</td>\
						</tr>\
						</table>\
						</span>';\n";

				// JSON
				if (iff.getItemGroupIdentify(common->_typeid) == iff::SET_ITEM)
					setitem_itens = getAllItemInSetItem(common->_typeid);
				else if (!setitem_itens.empty())
					setitem_itens.clear();

				map_item_json[type_name].push_back({ id, common->_typeid, common->name, common->icon, (desc != nullptr ? desc->description : "No Description"), type_name, !setitem_itens.empty(), setitem_itens });

				// String
				map_str[type_name].push_back(table);
			}

			std::string item_category;

			table = "";

			for (auto& el : map_str) {

				if (item_category.empty())
					item_category = "define(\"ITEM_CATEGORY\", ['" + el.first + "'";
				else
					item_category += ", '" + el.first + "'";
					
				//table += "\tfunction category_" + el.first + "() {\n\t// Type Name: " + el.first + "\n";

				table += "\techo '<span name=\"" + el.first + "\" class=\"category\">" + el.first + "</span>';\n";

				for (auto& el2 : el.second)
					table += "\t" + el2;

				//table += "}\n";
			}

			item_category += "]);\n";

			table.insert(table.begin(), item_category.begin(), item_category.end());

			//_smp::message_pool::getInstance().push(new message(table, CL_ONLY_FILE_LOG_TEST));

			// Save JSON
			auto newlineReplace = [](std::string _s) {

				// Check Barra inversa
				auto index = _s.find('\\');

				while (index != std::string::npos) {
					_s.replace(index, 1, "");

					index = _s.find('\\', index);
				}

				// Check New Line
				index = _s.find('\n');

				while (index != std::string::npos) {
					_s.replace(index, 1, "\\n");

					index = _s.find('\n', index + 2);
				}

				// Check Aspas composta
				index = _s.find('\"');

				while (index != std::string::npos) {
					_s.replace(index, 1, "\\\"");

					index = _s.find('\"', index + 2);
				}

				return _s;
			};

			bool b = false, c = false, d = false;

			std::string json = "[";

			for (auto& el : map_item_json) {

				c = false;

				if (!b)
					b = true;
				else
					json += ",";

				json += "{\"" + el.first + "\":[";

				for (auto& ell : el.second) {

					d = false;

					if (!c)
						c = true;
					else
						json += ",";

					json += "{\"id\":" + std::to_string(ell.id);
					json += ",\"_typeid\":" + std::to_string(ell._typeid);
					json += ",\"type\":\"" + ell.type + "\"";
					json += ",\"is_setitem\":" + std::string(ell.is_setitem && !ell.v_setitem_itens.empty() ? "true" : "false");
					json += ",\"name\":\"" + newlineReplace(ell.name) + "\"";
					json += ",\"icon\":\"" + newlineReplace(ell.icon) + "\"";
					json += ",\"description\":\"" + newlineReplace(ell.description) + "\"";

					if (ell.is_setitem && !ell.v_setitem_itens.empty()) {

						json += ",\"set_item\":[";

						for (auto& el2 : ell.v_setitem_itens) {

							if (!d)
								d = true;
							else
								json += ",";

							json += "{\"typeid\":" + std::to_string(el2._typeid);

							json += ",\"icon\":\"" + newlineReplace(el2.icon) + "\"}";
						}

						json += "]";
					}

					json += "}";
				}

				json += "]}";
			}

			json += "]";

			_smp::message_pool::getInstance().push(new message(json, CL_ONLY_FILE_LOG_TEST));

			id = 0ul;

			IFF::Card::CARD_SUB_TYPE type;

			type_name = "";

			map_str.clear();

			auto getTypeCardName = [](IFF::Card::CARD_SUB_TYPE _type) {

				switch (_type) {
				case IFF::Card::T_CADDIE:
					return "Caddie";
				case IFF::Card::T_CHARACTER:
					return "Character";
				case IFF::Card::T_SPECIAL:
					return "Special";
				case IFF::Card::T_NPC:
					return "NPC";
				}

				return "Unknown";
			};

			auto getRarityCard = [](unsigned char _type, bool _extended = false) {

				switch (_type) {
				case 0:
					return (_extended ? "Normal" : "N");
				case 1:
					return (_extended ? "Rare": "R");
				case 2:
					return (_extended ? "Super Rare" : "SR");
				case 3:
					return (_extended ? "Secret" : "SC");
				}

				return "Unknown";
			};

			std::string rarity[2];

			struct cardjson {
				unsigned long id;
				unsigned long _typeid;
				unsigned short volume;
				unsigned char rarity;
				std::string name;
				std::string icon;
				std::string description;
			};

			std::map< std::string, std::map< std::string, std::vector< std::string > > > mmap_str;
			std::map< std::string, std::map< std::string, std::vector< cardjson > > > map_json;

			for (auto& el : iff.getCard()) {

				type = IFF::Card::CARD_SUB_TYPE(iff.getItemSubGroupIdentify22(el.second._typeid));

				if (IFF::Card::T_CADDIE == type || IFF::Card::T_CHARACTER == type || IFF::Card::T_SPECIAL == type || IFF::Card::T_NPC == type) {

					table = "";

					++id;

					type_name = getTypeCardName(type);
					rarity[0] = getRarityCard(el.second.tipo);
					rarity[1] = getRarityCard(el.second.tipo, true);

					table += "echo '<span name=\"" + func(std::string(el.second.name)) + "\" id=\"" + std::to_string(el.second._typeid) + "\" category=\"" + type_name + ", " + rarity[1] + "\" class=\"lid\"><table width=\"100%\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\">\
						<tr>\
						<td align=\"center\" vAlign=\"middle\">\
						<table width=\"100%\" border=\"0\" cellspacing=\"2\" cellpadding=\"0\">\
						<tr>\
						<td style=\"width: 25px; padding-left: 5px;\">" + std::to_string(id) + "</td>\
						<td align=\"left\" width=\"20%\">\
						<img src=\"https://localhost:447/img/item/'.urlencode('" + func(std::string(el.second.icon)) + ".png').'\" alt=\"";

					if ((desc = iff.findDesc(el.second._typeid)) != nullptr)
						table += func(std::string(desc->description));
					else
						table += "No Description";

					table += "\" /></td>\
							  <td width=\"40\" align=\"left\"> Vol." + std::to_string(el.second.volume) + "</td>\
							  <td width=\"25\">" + rarity[0] + "</td>\
							  <td>" + func(std::string(el.second.name));

					table += "</td>\
						</tr>\
						</table>\
						</td>\
						</tr>\
						</table>\
						</span>';\n";

					// JSON
					map_json[rarity[1]][type_name].push_back({ id, el.second._typeid, el.second.volume, el.second.tipo, el.second.name, el.second.icon, (desc != nullptr ? desc->description : "No Description") });

					// string
					mmap_str[rarity[1]][type_name].push_back(table);

				}
			}

			item_category = "";

			table = "";
			
			for (auto& el : mmap_str) {

				table += "\techo '<span name=\"" + el.first + "\" class=\"rarity\">" + el.first + "</span>';\n";

				for (auto& ell : el.second) {

					if (item_category.empty())
						item_category = "define(\"ITEM_CATEGORY\", ['" + ell.first + "'";
					else
						item_category += ", '" + ell.first + "'";

					//table += "\tfunction category_" + ell.first + "() {\n\t// Type Name: " + ell.first + "\n";

					table += "\techo '<span name=\"" + ell.first + "\" class=\"category\">" + ell.first + "</span>';\n";

					for (auto& el2 : ell.second)
						table += "\t" + el2;

					//table += "}\n";
				}
			}

			item_category += "]);\n";

			table.insert(table.begin(), item_category.begin(), item_category.end());

			//_smp::message_pool::getInstance().push(new message(table, CL_ONLY_FILE_LOG_TEST));

			// Save JSON
			b = false, c = false, d = false;

			json = "[";

			for (auto& el : map_json) {

				c = false;

				if (!b)
					b = true;
				else
					json += ",";

				json += "{\"" + el.first + "\":[";

				for (auto& ell : el.second) {

					d = false;

					if (!c)
						c = true;
					else
						json += ",";

					json += "{\"" + ell.first + "\":[";

					for (auto& el2 : ell.second) {

						if (!d)
							d = true;
						else
							json += ",";
						
						json += "{\"id\":" + std::to_string(el2.id);
						json += ",\"_typeid\":" + std::to_string(el2._typeid);
						json += ",\"volume\":" + std::to_string(el2.volume);
						json += ",\"rarity\":" + std::to_string((unsigned short)el2.rarity);
						json += ",\"name\":\"" + newlineReplace(el2.name) + "\"";
						json += ",\"icon\":\"" + newlineReplace(el2.icon) + "\"";
						json += ",\"description\":\"" + newlineReplace(el2.description) + "\"}";
					}

					json += "]}";

				}

				json += "]}";
			}

			json += "]";

			//_smp::message_pool::getInstance().push(new message(json, CL_ONLY_FILE_LOG_TEST));
		
		}else if (escolha == 10) {

			auto& cards = iff.getCard();

			std::vector< std::string > enable_ps, disable_ps;
			std::string tmp = "";

			IFF::Card::CARD_SUB_TYPE type;

			for (auto& el : cards) {

				type = IFF::Card::CARD_SUB_TYPE(iff.getItemSubGroupIdentify22(el.second._typeid));

				if (IFF::Card::T_CADDIE == type || IFF::Card::T_CHARACTER == type || IFF::Card::T_SPECIAL == type || IFF::Card::T_NPC == type || IFF::Card::T_PACK == type) {

					tmp = "typeid: " + std::to_string(el.second._typeid) + " (0x" + hex_util::ltoaToHex(el.second._typeid)
						+ ")\nName: " + std::string(el.second.name) + "\nVolume: " + std::to_string(el.second.volume)
						+ "\nBlock Personal Shop and Mail Box: " + (el.second.shop.flag_shop.uFlagShop.stFlagShop.block_mail_and_personal_shop ? "True" : "False");

					if (el.second.shop.flag_shop.uFlagShop.stFlagShop.can_send_mail_and_personal_shop)
						enable_ps.push_back(tmp);
					else
						disable_ps.push_back(tmp);
				}

				// Ativa card Volume 2 ao personal shop
				if (el.second.volume == 2 || el.second._typeid == 0x7C40003B || el.second._typeid == 0x7C800009 || el.second._typeid == 0x7C800047)
					el.second.shop.flag_shop.uFlagShop.stFlagShop.can_send_mail_and_personal_shop = 1u;

				if (IFF::Card::T_PACK == type) {
					
					_smp::message_pool::getInstance().push(new message(std::string("Card Pack:\nTypeid: ") + std::to_string(el.second._typeid) + " (0x" 
							+ hex_util::ltoaToHex(el.second._typeid) + ")\nName: " + std::string(el.second.name) + "\nPrice: " 
							+ std::to_string(el.second.shop.price) + "\tDescPrice: " + std::to_string(el.second.shop.desconto)
							+ "\nCP Shop: " + (el.second.shop.flag_shop.uFlagShop.stFlagShop.is_cash ? "CP" : "Pang")
							+ " -\t Saleable: " + (el.second.shop.flag_shop.uFlagShop.stFlagShop.is_saleable ? "Yes" : "No")
							+ " -\t Giftable: " + (el.second.shop.flag_shop.uFlagShop.stFlagShop.is_giftable ? "Yes" : "No")
							+ "\nDate Start: " + _formatDate(el.second.date.date[0]) + "\t-\tEnd: " + _formatDate(el.second.date.date[1]), CL_ONLY_FILE_LOG_TEST));

					if (el.second.shop.price == 15 && !el.second.shop.flag_shop.uFlagShop.stFlagShop.is_saleable) {
						el.second.shop.price = 5;
						el.second.shop.flag_shop.uFlagShop.stFlagShop.is_saleable = 1u;

						if (el.second.date.active_date || !isEmpty(el.second.date.date[0]) || !isEmpty(el.second.date.date[1]))
							el.second.date.clear();
					}
				}
			}

			iff.saveCard();

			// Enable PS
			_smp::message_pool::getInstance().push(new message("Enable Count: " + std::to_string(enable_ps.size()), CL_ONLY_FILE_LOG_TEST));

			for (auto& el : enable_ps)
				_smp::message_pool::getInstance().push(new message(el, CL_ONLY_FILE_LOG_TEST));

			// Disable PS
			_smp::message_pool::getInstance().push(new message("Disable Count: " + std::to_string(disable_ps.size()), CL_ONLY_FILE_LOG_TEST));

			for (auto& el : disable_ps)
				_smp::message_pool::getInstance().push(new message(el, CL_ONLY_FILE_LOG_TEST));
		
		}else if (escolha == 11) {

			// Lambda print all tiki_pts >= 15
			constexpr auto printAllTikiPtsMoreOrEqual15 = [](auto& _map) {

				bool update = false;

				for (auto& el : _map) {

					if (el.second.tiki.tiki_pts >= 10) {

						_smp::message_pool::getInstance().push(new message("Item[TYPEID=" + std::to_string(el.second._typeid) + "(0x"
								+ hex_util::ltoaToHex(el.second._typeid) + "), TIKI_PTS=" + std::to_string(el.second.tiki.tiki_pts)
								+ ", MILAGE=" + std::to_string(el.second.tiki.milage_pts) + "]", CL_FILE_LOG_TEST_AND_CONSOLE));

						if ((el.second.tiki.tiki_pts * 1000ul) != (el.second.tiki.milage_pts)) {

							el.second.tiki.tiki_pts = el.second.tiki.milage_pts / 1000ul;

							if (el.second.tiki.tiki_pts == 0ul) {

								el.second.tiki.tiki_pts = 1ul;
								el.second.tiki.qnt_per_tikis_pts = (el.second.tiki.milage_pts != 0ul) ? 1000ul / el.second.tiki.milage_pts : 1ul;

							}else
								el.second.tiki.qnt_per_tikis_pts = 1ul;

							update = true;
						}
					}
				}

				return update;
			};

			if (printAllTikiPtsMoreOrEqual15(iff.getCharacter()))
				iff.saveCharacter();
			if (printAllTikiPtsMoreOrEqual15(iff.getPart()))
				iff.savePart();
			if (printAllTikiPtsMoreOrEqual15(iff.getAuxPart()))
				iff.saveAuxPart();
			if (printAllTikiPtsMoreOrEqual15(iff.getCaddie()))
				iff.saveCaddie();
			if (printAllTikiPtsMoreOrEqual15(iff.getClubSet()))
				iff.saveClubSet();
			if (printAllTikiPtsMoreOrEqual15(iff.getBall()))
				iff.saveBall();
			if (printAllTikiPtsMoreOrEqual15(iff.getItem()))
				iff.saveItem();
			if (printAllTikiPtsMoreOrEqual15(iff.getMascot()))
				iff.saveMascot();
			if (printAllTikiPtsMoreOrEqual15(iff.getCard()))
				iff.saveCard();
			if (printAllTikiPtsMoreOrEqual15(iff.getSkin()))
				iff.saveSkin();
		
		}else if (escolha == 12) {

			std::vector< unsigned long > item_name_print{ 0x08008828,0x08008829,0x0804683d,0x0804683e,0x0808882a,0x0808882b,0x080c8042,0x080c8043,0x0810602b,0x0810602c,0x08148042,0x08148043,0x0818603f,0x08186040,0x081c8030,0x081c8031,0x08208049,0x0820804a,0x08248044,0x08248045,0x0828803d,0x0828803e,0x082c880f,0x082c8810,0x0830800b,0x0830800c,0x0838800c,0x0838800d,0x2420a07b,0x2420e039,0x24216003,0x24218003,0x24218004,0x1000007e };

			for (auto& el : item_name_print) {

				auto common = iff.findCommomItem(el);

				if (common != nullptr)
					_smp::message_pool::getInstance().push(new message("INSERT INTO [pangya].[pangya_gacha_jp_all_item_list]([typeid],[name],[char_type]) VALUES(" 
							+ std::to_string(common->_typeid) + ", N'" 
							+ std::string(common->name) + "', "
							+ std::to_string([](unsigned long _type) {
								
								if (_type > 10) {
									_type = (_type % 10) - 1;
								}

								return _type;
							}(iff.getItemCharIdentify(common->_typeid))) + ")"
						, CL_FILE_LOG_TEST_AND_CONSOLE));
			}

			for (auto& el : item_name_print) {

				auto common = iff.findCommomItem(el);

				if (common != nullptr)
					_smp::message_pool::getInstance().push(new message("INSERT INTO [pangya].[pangya_gacha_jp_item_list](active, gacha_num, typeid_1, qnty_1, rarity_type) VALUES(1, 2, "
							+ std::to_string(common->_typeid) + ", 1, 0)", CL_FILE_LOG_TEST_AND_CONSOLE));
			}

			// Itens db json
			IFF::Desc *desc = nullptr;
			IFF::Base *common = nullptr;

			unsigned long id = 0ul;

			size_t index = 0;

			auto func = [](std::string _s) {

				auto index = _s.find('\'');

				while (index != std::string::npos) {
					_s.replace(index, 1, "\\'");

					index = _s.find('\'', index + 2);
				}

				return _s;
			};

			auto getTypeName = [](unsigned long _typeid) {

				switch (sIff::getInstance().getItemGroupIdentify(_typeid)) {
				case iff::AUX_PART:
					return "Ring";
				case iff::BALL:
					return "Ball";
				case iff::CADDIE:
					return "Caddie";
				case iff::CARD:
					return "Card";
				case iff::CHARACTER:
					return "Character";
				case iff::CLUBSET:
					return "Clubset";
				case iff::ITEM:
					return "Item";
				case iff::MASCOT:
					return "Mascot";
				case iff::PART:
					return "Outfit";
				case iff::SET_ITEM:
					return "Setitem";
				}

				return "Unknown";
			};

			struct setiteminfo {
				unsigned long _typeid;
				std::string icon;
			};

			struct itemjson {
				unsigned long id;
				unsigned long _typeid;
				std::string name;
				std::string icon;
				std::string description;
				std::string type;
				bool is_setitem;
				std::vector< setiteminfo > v_setitem_itens;
			};

			auto getAllItemInSetItem = [&](unsigned long _typeid) {

				std::vector< setiteminfo > items;

				if (iff.getItemGroupIdentify(_typeid) == iff::SET_ITEM) {

					auto setitem = iff.findSetItem(_typeid);

					if (setitem != nullptr) {

						for (auto i = 0ul; i < (sizeof(setitem->packege.item_typeid) / sizeof(setitem->packege.item_typeid[0])); ++i) {

							if (setitem->packege.item_typeid[i] > 0ul && iff.getItemGroupIdentify(setitem->packege.item_typeid[i]) != iff::CHARACTER) {

								auto common_item = iff.findCommomItem(setitem->packege.item_typeid[i]);

								if (common_item != nullptr)
									items.push_back({ setitem->packege.item_typeid[i], common_item->icon });
							}
						}
					}
				}

				return items;
			};

			std::string type_name;

			std::map< std::string, std::vector< std::string > > map_str;
			std::map< std::string, std::vector< itemjson > > map_item_json;

			std::vector< setiteminfo > setitem_itens;

			for (auto& el : item_name_print) {

				if ((common = iff.findCommomItem(el)) != nullptr) {

					++id;

					type_name = getTypeName(el);

					desc = iff.findDesc(el);

					// JSON
					if (iff.getItemGroupIdentify(el) == iff::SET_ITEM)
						setitem_itens = getAllItemInSetItem(el);
					else if (!setitem_itens.empty())
						setitem_itens.clear();

					map_item_json[type_name].push_back({ id, el, common->name, common->icon, (desc != nullptr ? desc->description : "No Description"), type_name, !setitem_itens.empty(), setitem_itens });
				}
			}

			// Save JSON
			auto newlineReplace = [](std::string _s) {

				// Check Barra inversa
				auto index = _s.find('\\');

				while (index != std::string::npos) {
					_s.replace(index, 1, "");

					index = _s.find('\\', index);
				}

				// Check New Line
				index = _s.find('\n');

				while (index != std::string::npos) {
					_s.replace(index, 1, "\\n");

					index = _s.find('\n', index + 2);
				}

				// Check Aspas composta
				index = _s.find('\"');

				while (index != std::string::npos) {
					_s.replace(index, 1, "\\\"");

					index = _s.find('\"', index + 2);
				}

				return _s;
			};

			bool b = false, c = false, d = false;

			std::string json = "[";

			for (auto& el : map_item_json) {

				c = false;

				if (!b)
					b = true;
				else
					json += ",";

				json += "{\"" + el.first + "\":[";

				for (auto& ell : el.second) {

					d = false;

					if (!c)
						c = true;
					else
						json += ",";

					json += "{\"id\":" + std::to_string(ell.id);
					json += ",\"_typeid\":" + std::to_string(ell._typeid);
					json += ",\"type\":\"" + ell.type + "\"";
					json += ",\"is_setitem\":" + std::string(ell.is_setitem && !ell.v_setitem_itens.empty() ? "true" : "false");
					json += ",\"name\":\"" + newlineReplace(ell.name) + "\"";
					json += ",\"icon\":\"" + newlineReplace(ell.icon) + "\"";
					json += ",\"description\":\"" + newlineReplace(ell.description) + "\"";

					if (ell.is_setitem && !ell.v_setitem_itens.empty()) {

						json += ",\"set_item\":[";

						for (auto& el2 : ell.v_setitem_itens) {

							if (!d)
								d = true;
							else
								json += ",";

							json += "{\"typeid\":" + std::to_string(el2._typeid);

							json += ",\"icon\":\"" + newlineReplace(el2.icon) + "\"}";
						}

						json += "]";
					}

					json += "}";
				}

				json += "]}";
			}

			json += "]";

			_smp::message_pool::getInstance().push(new message(json, CL_ONLY_FILE_LOG_TEST));
		
		}else if (escolha == 13) {

			std::vector< unsigned long > cutin_enable_shop{ 0x39400211,0x39400212,0x39400213,0x39400214,0x39400215,0x39400216,0x39400217,0x39400218,0x39400219,0x3940021a,0x3940021b,0x3940021c,0x3940021d,0x3940021e,0x3940021f,0x39400220,0x39400221,0x39400222,0x39400223,0x39400224,0x39400225,0x39400226,0x39400227,0x39400228,0x39400229,0x3940022a,0x3940022b,0x3940022c,0x3940022d,0x3940022e,0x3940022f,0x39400230,0x39400231,0x39400232,0x39400233,0x39400234,0x39400235,0x39400236,0x39400237,0x39400238,0x39400239,0x3940023a,0x3940023b,0x3940023c,0x3940023d,0x3940023e,0x3940023f,0x39400240,0x39400241,0x39400242,0x39400243,0x39400244,0x39400245,0x39400246,0x39400247,0x39400248,0x39400249,0x3940024a,0x3940024b,0x3940024c,0x3940024d,0x3940024e,0x3940024f,0x39400250,0x39400251,0x39400252,0x39400253,0x39400254,0x39400255,0x39400256,0x39400257,0x39400258,0x39400259,0x3940025a,0x3940025b,0x3940025c,0x3940025d,0x3940025e,0x3940025f,0x39400260,0x39400261,0x39400262,0x39400263,0x39400264 };

			for (auto& el : cutin_enable_shop) {

				auto common = iff.findCommomItem(el);

				if (common != nullptr) {

					common->shop.flag_shop.uFlagShop.stFlagShop.is_saleable = 1ul;
					common->shop.price = 5;

				}
			}

			iff.saveSkin();
		
		}else if (escolha == 14) {
		
			std::vector< std::string > papel_shop_rare_num_28
			{
				"z_tl_02_03", "z_tl_02_02", "common_white_feather", "common_tl_04_01", "common_tl_03_01", "common_tl_01_01", "common_silver_queens_crown", "common_silver_prince_crown", "common_red_feather", "common_ha_a_z52_01", "common_ha_a_z01_01",
				"common_flush_02", "common_flush_01", "common_fc_z28_02", "common_fc_z28_01", "common_fc_z22-02", "common_fc_z22-01", "common_fc_z18", "common_fc_a_z37-03", "common_fc_a_z37-02", "common_fc_a_z37-01", "common_black_devilear"
			};

			// Encontra all PartItem by Icon name
			auto lambda_find_all_item_by_icon = [&iff](std::string& _icon) -> std::vector< IFF::Part* > {

				std::vector< IFF::Part* > v_part;

				for (auto& el : iff.getPart()) {

					if (_stricmp(el.second.icon, _icon.c_str()) == 0)
						v_part.push_back( &el.second );
				}

				return v_part;
			};

			std::vector< IFF::Part* > v_part;
			std::string insert_db = "";

			for (auto& icon : papel_shop_rare_num_28) {

				if (!(v_part = lambda_find_all_item_by_icon(icon)).empty()) {

					insert_db += "--" + icon + "\n";

					for (auto& part : v_part) {

						if (part == nullptr)
							continue;

						insert_db += "INSERT INTO pangya.pangya_papel_shop_item([Nome], [typeid], [probabilidade], [numero], [tipo], [active]) VALUES(";

						insert_db += "N'" + std::string(part->name) + "',";
						insert_db += std::to_string(part->_typeid) + ",";

						insert_db += "100, 28, 2, 1);\n";
					}
				}
			}

			_smp::message_pool::getInstance().push(new message(insert_db, CL_ONLY_FILE_LOG_TEST));
		}

	}catch (exception& e) {

		std::cout << e.getFullMessageError() << std::endl;
	}

	try {

		while (!_smp::message_pool::getInstance().empty())
			_smp::message_pool::getInstance().console_log(1000);
	
	}catch (exception& e) {

		std::cout << e.getFullMessageError() << std::endl;
	}

#ifdef _DEBUG
	system("pause");
#endif // _DEBUG


	return 0;
}

// Executar programa: Ctrl + F5 ou Menu Depurar > Iniciar Sem Depuração
// Depurar programa: F5 ou menu Depurar > Iniciar Depuração

// Dicas para Começar: 
//   1. Use a janela do Gerenciador de Soluções para adicionar/gerenciar arquivos
//   2. Use a janela do Team Explorer para conectar-se ao controle do código-fonte
//   3. Use a janela de Saída para ver mensagens de saída do build e outras mensagens
//   4. Use a janela Lista de Erros para exibir erros
//   5. Ir Para o Projeto > Adicionar Novo Item para criar novos arquivos de código, ou Projeto > Adicionar Item Existente para adicionar arquivos de código existentes ao projeto
//   6. No futuro, para abrir este projeto novamente, vá para Arquivo > Abrir > Projeto e selecione o arquivo. sln


//int erro = 0, flag_zip = ZIP_FL_ENC_UTF_8;
//zip *z = NULL;
//zip_source *s = NULL;
//
//if (flag_rezip_files)
//	flag_zip |= ZIP_FL_OVERWRITE;
//
//std::string s_tmp, arq_name;
//	
//// copia o path copia para string de wstring
//s_tmp.assign(path_copy.begin(), path_copy.end());
//
//// cria os arquivo zip, se já tiver pula para o próximo
//for (unsigned int i = 0; i < list_tmp.size(); i++) {
//	std::cout << "Zipando arquivo: " << list_tmp[i] << std::endl;
//	arq_name = list_tmp[i].substr(list_tmp[i].find_last_of("/\\")+1);
//	list_tmp[i] = s_tmp + list_tmp[i];
//
//	if ((z = zip_open((list_tmp[i] + ".zip").c_str(), ZIP_CREATE, &erro)) != NULL) {
//		if ((s = zip_source_file(z, list_tmp[i].c_str(), 0, 0)) == NULL || zip_file_add(z, arq_name.c_str(), s, flag_zip))
//			if (s != NULL)
//				zip_source_free(s);
//			else
//				std::cout << "Erro ao criar um arquivo zip. File: " << list_tmp[i] << std::endl;
//	}else
//		std::cout << "ErroZip : " << erro << "File: " << list_tmp[i] << std::endl;
//
//	if (z != NULL)
//		zip_close(z);
//}