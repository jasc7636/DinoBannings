#include <fstream>
#include <string>
#include <vector>
#include <API/ARK/Ark.h>
#include "json.hpp"
#pragma comment(lib, "ArkApi.lib")

namespace DinoBannings
{
	nlohmann::json config;

	std::string FORCEINLINE getMapName()
	{
		FString mapName;
		ArkApi::GetApiUtils().GetWorld()->GetMapName(&mapName);
		return ArkApi::Tools::Utf8Encode(*mapName);
	}

	// load config from ./ArkApi/Plugins/DinoBannings/config.json
	void load_config()
	{
		std::ifstream config_file{ ArkApi::Tools::GetCurrentDir() + "/ArkApi/Plugins/DinoBannings/config.json" };
		if (!config_file.is_open())
		{
			throw std::runtime_error("Can't open config.json");
		}
		config_file >> config;
		config_file.close();
	}

	
	// Hook AllowDownloadDino_Implementation function
	// Gets called when a dino is downloaded, uncryoed or unsoulballed
	DECLARE_HOOK(AShooterGameState_AllowDownloadDino_Implementation, bool, AShooterGameState*, TSubclassOf<APrimalDinoCharacter>);
	// Gets called when a dino starts to fly or if it lands
	DECLARE_HOOK(APrimalDinoCharacter_SetFlight, void, APrimalDinoCharacter*, bool, bool);

	void load()
	{
		DinoBannings::load_config();
		if (config.value("EnableChatCommand", false))
		{
			ArkApi::GetCommands().AddChatCommand(config.value("ChatCommand", "/disallowed_dinos").c_str(),
				[](AShooterPlayerController* player_controller, FString* message, EChatSendMode::Type) {
					std::string disallowed_release;
					for (auto& dino : config.value("/DisallowRelease"_json_pointer / getMapName(), std::vector<std::string>()))
					{
						if (!disallowed_release.empty())
							disallowed_release.append(", ");
						disallowed_release.append(dino);
					}

					std::string disallowed_flying;
					for (auto& dino : config.value("/PreventFlying"_json_pointer / getMapName(), std::vector<std::string>()))
					{
						if (!disallowed_flying.empty())
							disallowed_flying.append(", ");
						disallowed_flying.append(dino);
					}

					if (!disallowed_release.empty())
						ArkApi::GetApiUtils().SendChatMessage(player_controller, "disallowed release", disallowed_release.c_str());
					if (!disallowed_flying.empty())
						ArkApi::GetApiUtils().SendChatMessage(player_controller, "disallowed flying", disallowed_flying.c_str());
				});
		}
		ArkApi::GetHooks().SetHook("AShooterGameState.AllowDownloadDino_Implementation", &Hook_AShooterGameState_AllowDownloadDino_Implementation, &AShooterGameState_AllowDownloadDino_Implementation_original);
		ArkApi::GetHooks().SetHook("APrimalDinoCharacter.SetFlight", &Hook_APrimalDinoCharacter_SetFlight, &APrimalDinoCharacter_SetFlight_original);
	}

	void unload()
	{
		Log::GetLog()->info("Remove hooks");
		if (config.value("EnableChatCommand", false))
		{
			ArkApi::GetCommands().RemoveChatCommand(config.value("ChatCommand", "/disallowed_dinos").c_str());
		}
		ArkApi::GetHooks().DisableHook("AShooterGameState.AllowDownloadDino_Implementation", &Hook_AShooterGameState_AllowDownloadDino_Implementation);
		ArkApi::GetHooks().DisableHook("APrimalDinoCharacter.SetFlight", &Hook_APrimalDinoCharacter_SetFlight);
	}

	bool Hook_AShooterGameState_AllowDownloadDino_Implementation(AShooterGameState* _this, TSubclassOf<APrimalDinoCharacter> TheDinoClass)
	{
		if (TheDinoClass.uClass != nullptr)
		{
			FString dinoClassName = ArkApi::GetApiUtils().GetClassBlueprint(TheDinoClass.uClass);

			for (auto& dino : config.value("/DisallowRelease"_json_pointer / getMapName(), std::vector<std::string>()))
			{
				if (dinoClassName.Contains(dino.c_str()))
				{
					return false;
				}
			}
		}

		return AShooterGameState_AllowDownloadDino_Implementation_original(_this, TheDinoClass);
	}

	void Hook_APrimalDinoCharacter_SetFlight(APrimalDinoCharacter* _this, bool bFly, bool bCancelForceLand)
	{
		if (_this != nullptr)
		{
			FString dinoClassName = ArkApi::GetApiUtils().GetBlueprint(_this);

			if (bFly)
			{
				for (auto& dino : config.value(nlohmann::json_pointer("/PreventFlying"_json_pointer / getMapName()), std::vector<std::string>()))
				{
					if (dinoClassName.Contains(dino.c_str()))
					{
						// Get player and send message
						AShooterCharacter* rider = _this->RiderField().Get();
						if (rider && rider->GetInstigatorController()->IsA(AShooterPlayerController::GetPrivateStaticClass()))
						{
							AShooterPlayerController* player_controller = static_cast<AShooterPlayerController*>(rider->GetInstigatorController());
							if (player_controller)
							{
								ArkApi::GetApiUtils().SendNotification(player_controller, { 1.0f, 0.0f, 0.0f }, 2.0f, 2.0f, nullptr, config.value("/PreventFlying/PlayerMessage"_json_pointer, "").c_str());
							}
						}

						return;
					}
					
				}			
			}
		}

		APrimalDinoCharacter_SetFlight_original(_this, bFly, bCancelForceLand);
	}
};

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		DinoBannings::load();
		break;
	case DLL_PROCESS_DETACH:
		DinoBannings::unload();
		break;
	}
	return TRUE;
}
