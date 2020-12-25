// Copyright 2020 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// source tree.

#include "ESDConnectionManager.h"
#include "ESDSDKDefines.h"
#include "ESDConnectionManager.h"
#include "EPLJSONUtils.h"

#include "StreamDeckPlugin.h"

#include <iostream>

void DebugPrint(const char *msg) {
	std::cout << msg << std::endl;
}

int main(int argc, const char **argv) {
	if (argc != 9)
	{
		DebugPrint("Invalid number of parameters instead of 9\n");
		return 1;
	}
	
	int port = 0;
	std::string pluginUUID;
	std::string registerEvent;
	std::string info;
	
	for(int argumentIndex = 0 ; argumentIndex < 4 ; argumentIndex++)
	{
		std::string parameter(argv[1 + 2 * argumentIndex]);
		std::string value(argv[1 + 2 * argumentIndex + 1]);
		
		if (parameter == kESDSDKPortParameter)
		{
			port = std::atoi(value.c_str());
		}
		else if (parameter == kESDSDKPluginUUIDParameter)
		{
			pluginUUID = value;
		}
		else if (parameter == kESDSDKRegisterEventParameter)
		{
			registerEvent = value;
		}
		else if (parameter == kESDSDKInfoParameter)
		{
			info = value;
		}
	}
	
	if(port == 0)
	{
		DebugPrint("Invalid port number\n");
		return 1;
	}

	if(pluginUUID.empty())
	{
		DebugPrint("Invalid plugin UUID\n");
		return 1;
	}

	if(registerEvent.empty())
	{
		DebugPrint("Invalid registerEvent\n");
		return 1;
	}

	if (info.empty())
	{
		DebugPrint("Invalid info\n");
		return 1;
	}

	// Create the plugin
	StreamDeckPlugin *plugin = new StreamDeckPlugin();

	// Initialize localization helper
	std::string language = "en";
	
	try
	{
		nlohmann::json infoJson = nlohmann::json::parse(info);
		nlohmann::json applicationInfo;
		if(EPLJSONUtils::GetObjectByName(infoJson, kESDSDKApplicationInfo, applicationInfo))
		{
			language = EPLJSONUtils::GetStringByName(applicationInfo, kESDSDKApplicationInfoLanguage, language);
		}
	}
	catch(...)
	{
	
	}
	
	// ESDLocalizer::Initialize(language);

	// Create the connection manager
	ESDConnectionManager *connectionManager = new ESDConnectionManager(port, pluginUUID, registerEvent, info, plugin);
		
	// Connect and start the event loop
	connectionManager->Run();

	return 0;
}
