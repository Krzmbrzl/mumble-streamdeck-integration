// Copyright 2021 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// source tree.

#include "MumblePlugin.h"
#include "ConnectionManager.h"

#include <boost/process.hpp>
#include <boost/algorithm/string.hpp>

#include <string>

namespace Mumble {
namespace StreamDeckIntegration {

	void MumblePlugin::keyDownForAction(const std::string &actionID, const std::string &context,
										const nlohmann::json &payload, const std::string &deviceID) {
		try {
			boost::filesystem::path cliPath = findCLI();
			const nlohmann::json action     = getJSONForAction(actionID);
			nlohmann::json response         = executeAction(action, cliPath);

			// Clear any potential text on the button
			m_connectionManager->api_setTitle("", context, kESDSDKTarget_HardwareAndSoftware);
			try {
				std::string responseType = response["response_type"].get< std::string >();
				if (responseType != "error") {
					m_connectionManager->api_logMessage("Successfully executed action " + actionID);
				} else {
					m_connectionManager->reportError("Error while executing action " + actionID + " "
													 + response["response"]["error_message"].get< std::string >());
				}
			} catch (const nlohmann::json::exception &e) {
				throw PluginException(std::string("JSON error: ") + e.what());
			}
		} catch (const PluginException &e) {
			m_connectionManager->reportError(e.what(), context);
		}
	}

	void MumblePlugin::keyUpForAction(const std::string &actionID, const std::string &context,
									  const nlohmann::json &payload, const std::string &deviceID) {}

	void MumblePlugin::willAppearForAction(const std::string &actionID, const std::string &context,
										   const nlohmann::json &payload, const std::string &deviceID) {}

	void MumblePlugin::willDisappearForAction(const std::string &actionID, const std::string &context,
											  const nlohmann::json &payload, const std::string &deviceID) {}

	void MumblePlugin::deviceDidConnect(const std::string &deviceID, const nlohmann::json &deviceInfo) {}

	void MumblePlugin::deviceDidDisconnect(const std::string &deviceID) {}

	void MumblePlugin::sendToPlugin(const std::string &actionID, const std::string &context,
									const nlohmann::json &payload, const std::string &deviceID) {}

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
			throw PluginException("Unknown action \"" + actionID + "\"");
		}

		return action;
	}

	boost::filesystem::path MumblePlugin::findCLI() {
		const std::string cli_name      = "mumble_json_bridge_cli";
		boost::filesystem::path cliPath = boost::process::search_path(cli_name);
		if (cliPath.empty()) {
			// Not found
			throw PluginException("Unable to locate \"" + cli_name + "\" binary. Are you sure it's in path");
		}

		return cliPath;
	}

	nlohmann::json MumblePlugin::executeAction(const nlohmann::json &action, const boost::filesystem::path &cliPath) {
		boost::process::ipstream stdout_stream;
		boost::process::ipstream stderr_stream;
		std::error_code launchErrorCode;
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

		// Trim contents
		boost::trim(stdout_content);
		boost::trim(stderr_content);

		int processExitCode = c.exit_code();

		if (launchErrorCode) {
			throw PluginException("Trying to launch external process resulted in non-zero exit code: "
								  + std::to_string(launchErrorCode.value()));
		}
		if (processExitCode) {
			std::string errorMsg = "Calling the CLI returned non-zero exit code: " + std::to_string(processExitCode);
			if (stderr_content.size() > 0) {
				errorMsg += " (\"" + stderr_content + "\")";
			}

			throw PluginException(errorMsg);
		}

		try {
			// Parse and return
			return nlohmann::json::parse(stdout_content);
		} catch (const nlohmann::json::parse_error &e) {
			throw PluginException(std::string("CLI returned malformed JSON: ") + e.what() + " (JSON: \""
								  + stdout_content + "\")");
		}
	}

}; // namespace StreamDeckIntegration
}; // namespace Mumble
