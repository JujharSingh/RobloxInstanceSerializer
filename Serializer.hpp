#pragma once

#include <iostream>
#include <sstream>
#include <vector>
#include <any>
#include <Windows.h>

#include "lua\lobject.hpp"

#define R_USERDATA 8
#define HEADER R"(<?xml version="1.0" encoding="utf-8" ?>
<roblox xmlns:xmime="http://www.w3.org/2005/05/xmlmime" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="http://www.roblox.com/roblox.xsd" version="4"
<External>null</External>
<External>nil</External>)"

using std::endl;

namespace Serializer {
	namespace Internal {
		struct Instance {
			typedef std::vector<std::pair<std::string, std::any>> Properties;
			typedef std::vector<Instance> Instances;

			std::string clazz;
			Properties properties;
			Instances children;
		};

		using Instances = Instance::Instances;
		using Properties = Instance::Properties;

		static Instance parseObject(TValue *obj) {
			if (obj->tt != R_USERDATA)
				throw new std::exception("Object not a userdata");
		}

		static std::string toXML(Properties& props) {
			std::ostringstream xml;

			xml << "<Properties>" << endl;

			for (auto it = props.begin(); it != props.end(); ++it) {
				auto prop = *it;
				xml << "<" << prop.first << ">";

				if (prop.second.type() == typeid(std::string)) {
					xml << std::any_cast<std::string>(prop.second);
				}
				else if (prop.second.type() == typeid(Properties)) { // nested properties
					auto val = std::any_cast<Properties>(prop.second);
					xml << toXML(val);
				}

				xml << "</" << prop.first << ">";
			}

			

			xml << "</Properties>" << endl;
			return xml.str();
		}

		static std::string toXML(Instance& ins) {
			std::ostringstream xml;

			xml << "<Item class=\"" << ins.clazz << "\">" << endl;
			
			if (!ins.properties.empty()) {
				xml << "<Properties>" << endl;
				xml << toXML(ins.properties);
				xml << "</Properties>" << endl;
			}
			
			// if children exist parse them too
			for (auto it = ins.children.begin(); it != ins.children.end(); ++it) {
				xml << toXML(*it);
			}

			xml << "</Item>" << endl;
			return xml.str();
		}
	}
	

	static std::ostringstream serialize(DWORD L, TValue *obj) {
		std::ostringstream out;

		out << HEADER << endl;

		Internal::Instance root = Internal::parseObject(obj);
		out << Internal::toXML(root);

		out << "</roblox>" << endl;
		return out;
	}

	static void deserialize() {

	}
}
