// Copyright 2020 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// source tree.

#ifndef MUMBLE_STREAMDECK_INTEGRATION_STREAMDECKPLUGIN_H_
#define MUMBLE_STREAMDECK_INTEGRATION_STREAMDECKPLUGIN_H_

#include "ESDBasePlugin.h"

class StreamDeckPlugin : public ESDBasePlugin {
public:
	StreamDeckPlugin() { }
	virtual ~StreamDeckPlugin() { }
	
	virtual void KeyDownForAction(const std::string& inAction, const std::string& inContext,
			const nlohmann::json &inPayload, const std::string& inDeviceID) override;
	virtual void KeyUpForAction(const std::string& inAction, const std::string& inContext,
			const nlohmann::json &inPayload, const std::string& inDeviceID) override;
	
	virtual void WillAppearForAction(const std::string& inAction, const std::string& inContext,
			const nlohmann::json &inPayload, const std::string& inDeviceID) override;
	virtual void WillDisappearForAction(const std::string& inAction, const std::string& inContext,
			const nlohmann::json &inPayload, const std::string& inDeviceID) override;
	
	virtual void DeviceDidConnect(const std::string& inDeviceID, const nlohmann::json &inDeviceInfo) override;
	virtual void DeviceDidDisconnect(const std::string& inDeviceID) override;

	virtual void SendToPlugin(const std::string& inAction, const std::string& inContext,
			const nlohmann::json &inPayload, const std::string& inDeviceID) override;

private:
	nlohmann::json getJSONForAction(const std::string &actionID) const;
};

#endif // MUMBLE_STREAMDECK_INTEGRATION_STREAMDECKPLUGIN_H_
