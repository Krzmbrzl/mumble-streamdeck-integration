// Copyright 2021 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// source tree.

#include "MumblePlugin.h"
#include "ConnectionManager.h"

#include <boost/filesystem.hpp>
#include <boost/process.hpp>

#include <string>

namespace Mumble {
namespace StreamDeckIntegration {

	void MumblePlugin::keyDownForAction(const std::string &inAction, const std::string &inContext,
										const nlohmann::json &inPayload, const std::string &inDeviceID) {
		// Search for the CLI executable in PATH
		boost::filesystem::path cliPath = boost::process::search_path("mumble_json_bridge_cli");
		if (cliPath.empty()) {
			// Not found
			m_connectionManager->api_setTitle("Error", inContext, kESDSDKTarget_HardwareAndSoftware);
			m_connectionManager->api_showAlertForContext(inContext);
			return;
		}



		boost::process::ipstream stdout_stream;
		boost::process::ipstream stderr_stream;
		std::error_code launchErrorCode;
		const nlohmann::json action = getJSONForAction(inAction);
		boost::process::child c(cliPath, "--json", action.dump(), boost::process::std_out > stdout_stream,
								boost::process::std_err > stderr_stream, launchErrorCode);

		std::string stdout_content;
		std::string stderr_content;
		std::string line;
		while (stdout_stream && std::getline(stdout_stream, line) && !line.empty()) {
			stdout_content += line;
		}
		while (stderr_stream && std::getline(stderr_stream, line) && !line.empty()) {
			stderr_content += line;
		}

		c.wait();

		int processExitCode = c.exit_code();

		if (launchErrorCode) {
			m_connectionManager->api_setTitle(std::string("LEC: ") + std::to_string(launchErrorCode.value()), inContext,
											  kESDSDKTarget_HardwareAndSoftware);
			m_connectionManager->api_showAlertForContext(inContext);
		} else {
			if (processExitCode == 0) {
				m_connectionManager->api_setTitle("", inContext, kESDSDKTarget_HardwareAndSoftware);
				try {
					nlohmann::json response = nlohmann::json::parse(stdout_content);

					std::string responseType = response["response_type"].get< std::string >();
					if (responseType != "error") {
						m_connectionManager->api_logMessage("Successfully executed action " + inAction);
					} else {
						m_connectionManager->api_logMessage(
							"Error while executing action " + inAction + " "
							+ response["response"]["error_message"].get< std::string >());
					}
				} catch (const nlohmann::json::exception &e) {
					m_connectionManager->api_logMessage(std::string("Malformed CLI response: ") + e.what());
				}
			} else {
				m_connectionManager->api_setTitle(std::string("PEC: ") + std::to_string(processExitCode), inContext,
												  kESDSDKTarget_HardwareAndSoftware);
				m_connectionManager->api_showAlertForContext(inContext);
			}
		}
	}

	void MumblePlugin::keyUpForAction(const std::string &inAction, const std::string &inContext,
									  const nlohmann::json &inPayload, const std::string &inDeviceID) {}

	void MumblePlugin::willAppearForAction(const std::string &inAction, const std::string &inContext,
										   const nlohmann::json &inPayload, const std::string &inDeviceID) {}

	void MumblePlugin::willDisappearForAction(const std::string &inAction, const std::string &inContext,
											  const nlohmann::json &inPayload, const std::string &inDeviceID) {}

	void MumblePlugin::deviceDidConnect(const std::string &inDeviceID, const nlohmann::json &inDeviceInfo) {}

	void MumblePlugin::deviceDidDisconnect(const std::string &inDeviceID) {}

	void MumblePlugin::sendToPlugin(const std::string &inAction, const std::string &inContext,
									const nlohmann::json &inPayload, const std::string &inDeviceID) {}

	nlohmann::json MumblePlugin::getJSONForAction(const std::string &actionID) const {
		nlohmann::json action;
		if (actionID == "info.mumble.mumble.actions.toggle_local_user_mute") {
			// clang-format off
		action = {
			{ "message_type", "operation" },
			{
				"message", {
					{ "operation", "toggle_local_user_mute" }
				}
			}
		};
			// clang-format on
		} else if (actionID == "info.mumble.mumble.actions.toggle_local_user_deaf") {
			// clang-format off
		action = {
			{ "message_type", "operation" },
			{
				"message", {
					{ "operation", "toggle_local_user_deaf" }
				}
			}
		};
			// clang-format on
		} else {
			throw std::invalid_argument(std::string("Unknown action ") + actionID);
		}

		return action;
	}

}; // namespace StreamDeckIntegration
}; // namespace Mumble
