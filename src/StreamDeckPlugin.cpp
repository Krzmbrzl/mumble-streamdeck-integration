// Copyright 2020 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// source tree.

#include "StreamDeckPlugin.h"
#include "ESDConnectionManager.h"

#include <boost/process.hpp>
#include <boost/filesystem.hpp>

#include <string>

void StreamDeckPlugin::KeyDownForAction(const std::string& inAction, const std::string& inContext,
			const nlohmann::json &inPayload, const std::string& inDeviceID) {
	// m_connectionManager->SetTitle(inContext, inContext, kESDSDKTarget_HardwareAndSoftware);

	// Search for the CLI executable in PATH
	boost::filesystem::path cliPath = boost::process::search_path("mumble_json_bridge_cli");
	if (cliPath.empty()) {
		// Not found
		m_connectionManager->SetTitle("Error", inContext, kESDSDKTarget_HardwareAndSoftware);
		m_connectionManager->ShowAlertForContext(inContext);
		return;
	}



	boost::process::ipstream stdout_stream;
	boost::process::ipstream stderr_stream;
	std::error_code launchErrorCode;
	const nlohmann::json action = getJSONForAction(inAction);
	boost::process::child c(cliPath, "--json", action.dump(),
			boost::process::std_out > stdout_stream, boost::process::std_err > stderr_stream, launchErrorCode);

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
		m_connectionManager->SetTitle(std::string("LEC: ") + std::to_string(launchErrorCode.value()), inContext, kESDSDKTarget_HardwareAndSoftware);
		m_connectionManager->ShowAlertForContext(inContext);
	} else {
		if (processExitCode == 0) {	
			m_connectionManager->SetTitle("", inContext, kESDSDKTarget_HardwareAndSoftware);
			try {
				nlohmann::json response = nlohmann::json::parse(stdout_content);

				std::string responseType = response["response_type"].get< std::string >();
				if (responseType != "error") {
					m_connectionManager->LogMessage("Successfully executed action " + inAction);
				} else {
					m_connectionManager->LogMessage("Error while executing action " + inAction + " "
							+ response["response"]["error_message"].get< std::string >());
				}
			} catch (const nlohmann::json::exception &e) {
				m_connectionManager->LogMessage(std::string("Malformed CLI response: ") + e.what());
			}
		} else {
			m_connectionManager->SetTitle(std::string("PEC: ") + std::to_string(processExitCode), inContext, kESDSDKTarget_HardwareAndSoftware);
			m_connectionManager->ShowAlertForContext(inContext);
		}
	}

}

void StreamDeckPlugin::KeyUpForAction(const std::string& inAction, const std::string& inContext,
			const nlohmann::json &inPayload, const std::string& inDeviceID) {
	// m_connectionManager->SetTitle(inAction, inContext, kESDSDKTarget_HardwareAndSoftware);
}
	
void StreamDeckPlugin::WillAppearForAction(const std::string& inAction, const std::string& inContext,
			const nlohmann::json &inPayload, const std::string& inDeviceID) {
}

void StreamDeckPlugin::WillDisappearForAction(const std::string& inAction, const std::string& inContext,
			const nlohmann::json &inPayload, const std::string& inDeviceID) {
}
	
void StreamDeckPlugin::DeviceDidConnect(const std::string& inDeviceID, const nlohmann::json &inDeviceInfo) {
}

void StreamDeckPlugin::DeviceDidDisconnect(const std::string& inDeviceID) {
}

void StreamDeckPlugin::SendToPlugin(const std::string& inAction, const std::string& inContext,
			const nlohmann::json &inPayload, const std::string& inDeviceID) {
}

nlohmann::json StreamDeckPlugin::getJSONForAction(const std::string &actionID) const {
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
