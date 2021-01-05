// Copyright 2021 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// source tree.

#ifndef MUMBLE_STREAMDECK_INTEGRATION_MUMBLEPLUGIN_H_
#define MUMBLE_STREAMDECK_INTEGRATION_MUMBLEPLUGIN_H_

#include "StreamDeckPlugin.h"

namespace Mumble {
namespace StreamDeckIntegration {

	class MumblePlugin : public StreamDeckPlugin {
	public:
		MumblePlugin() {}
		virtual ~MumblePlugin() {}

		virtual void keyDownForAction(const std::string &inAction, const std::string &inContext,
									  const nlohmann::json &inPayload, const std::string &inDeviceID) override;
		virtual void keyUpForAction(const std::string &inAction, const std::string &inContext,
									const nlohmann::json &inPayload, const std::string &inDeviceID) override;

		virtual void willAppearForAction(const std::string &inAction, const std::string &inContext,
										 const nlohmann::json &inPayload, const std::string &inDeviceID) override;
		virtual void willDisappearForAction(const std::string &inAction, const std::string &inContext,
											const nlohmann::json &inPayload, const std::string &inDeviceID) override;

		virtual void deviceDidConnect(const std::string &inDeviceID, const nlohmann::json &inDeviceInfo) override;
		virtual void deviceDidDisconnect(const std::string &inDeviceID) override;

		virtual void sendToPlugin(const std::string &inAction, const std::string &inContext,
								  const nlohmann::json &inPayload, const std::string &inDeviceID) override;

	private:
		nlohmann::json getJSONForAction(const std::string &actionID) const;
	};

};     // namespace StreamDeckIntegration
};     // namespace Mumble
#endif // MUMBLE_STREAMDECK_INTEGRATION_MUMBLEPLUGIN_H_
