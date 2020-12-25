//==============================================================================
/**
@file       ESDBasePlugin.h

@brief      Plugin base class

@copyright  (c) 2018, Corsair Memory, Inc.
			This source code is licensed under the MIT-style license found in the LICENSE file.

**/
//==============================================================================

#ifndef MUMBLE_STREAMDECK_INTEGRATION_ESDBASEPLUGIN_H_
#define MUMBLE_STREAMDECK_INTEGRATION_ESDBASEPLUGIN_H_

#include <string>

#include <nlohmann/json.hpp>

class ESDConnectionManager;

class ESDBasePlugin
{
public:
	ESDBasePlugin() { }
	virtual ~ESDBasePlugin() { }
	
	void SetConnectionManager(ESDConnectionManager * inConnectionManager) { m_connectionManager = inConnectionManager; }
	
	virtual void KeyDownForAction(const std::string& inAction, const std::string& inContext,
			const nlohmann::json &inPayload, const std::string& inDeviceID) = 0;
	virtual void KeyUpForAction(const std::string& inAction, const std::string& inContext,
			const nlohmann::json &inPayload, const std::string& inDeviceID) = 0;
	
	virtual void WillAppearForAction(const std::string& inAction, const std::string& inContext,
			const nlohmann::json &inPayload, const std::string& inDeviceID) = 0;
	virtual void WillDisappearForAction(const std::string& inAction, const std::string& inContext,
			const nlohmann::json &inPayload, const std::string& inDeviceID) = 0;
	
	virtual void DeviceDidConnect(const std::string& inDeviceID, const nlohmann::json &inDeviceInfo) = 0;
	virtual void DeviceDidDisconnect(const std::string& inDeviceID) = 0;

	virtual void SendToPlugin(const std::string& inAction, const std::string& inContext,
			const nlohmann::json &inPayload, const std::string& inDeviceID) = 0;
	
protected:
	ESDConnectionManager *m_connectionManager = nullptr;

};

#endif // MUMBLE_STREAMDECK_INTEGRATION_ESDBASEPLUGIN_H_
