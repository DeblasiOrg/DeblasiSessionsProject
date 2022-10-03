// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu.h"

#include "OnlineSubsystem.h"
#include "Components/Button.h"

void UMenu::MenuSetup(int32 NumberOfPublicConnections, FString TypeOfMatch)
{
	NumPublicConnections = NumberOfPublicConnections;
	MatchType = TypeOfMatch;
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;

	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* PC = World->GetFirstPlayerController();
		if (PC)
		{
			FInputModeUIOnly UIOnly;
			UIOnly.SetWidgetToFocus(TakeWidget());
			UIOnly.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PC->SetInputMode(UIOnly);
			PC->SetShowMouseCursor(true);
		}
	}

	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		Subsystem = GameInstance->GetSubsystem<UDeblasiSessionsSubsystem>();
		if (Subsystem)
		{
			Subsystem->DeblasiSessionsOnCreateSessionComplete.AddDynamic(this, &UMenu::OnCreateSession);
			Subsystem->DeblasiSessionsOnFindSessionsComplete.AddUObject(this, &UMenu::OnFindSession);
			Subsystem->DeblasiSessionsOnJoinSessionComplete.AddUObject(this, &UMenu::OnJoinSession);
			Subsystem->DeblasiSessionsOnDestroySessionComplete.AddDynamic(this, &UMenu::OnDestroySession);
			Subsystem->DeblasiSessionsOnStartSessionComplete.AddDynamic(this, &UMenu::OnStartSession);
		}
	}

	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
	{
		SessionInterface = OnlineSubsystem->GetSessionInterface();
	}
}

bool UMenu::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}
	
	if (HostButton)
	{
		HostButton->OnClicked.AddDynamic(this, &UMenu::HostButtonClicked);
	}
	if (JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this, &UMenu::JoinButtonClicked);
	}
	
	return true;
}

void UMenu::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld)
{
	Super::OnLevelRemovedFromWorld(InLevel, InWorld);

	MenuTeardown();
}

void UMenu::OnCreateSession(bool bSuccess)
{
	if (bSuccess)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.0f,
				FColor::Green,
				TEXT("Created session successfully"));
		}

		UWorld* World = GetWorld();
		if (World)
		{
			World->ServerTravel("/Game/ThirdPersonCPP/Maps/Lobby?listen");
		}
	} else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.0f,
				FColor::Red,
				TEXT("Failed to create session"));
		}
	}
}

void UMenu::OnFindSession(const TArray<FOnlineSessionSearchResult>& SearchResults, bool bSuccess)
{
	if (Subsystem == nullptr)
	{
		return;
	}
	
	for (auto Result : SearchResults)
	{
		FString Id = Result.GetSessionIdStr();
		FString User = Result.Session.OwningUserName;

		FString SettingsValue;
		Result.Session.SessionSettings.Get(FName("MatchType"), SettingsValue);
				
		if (SettingsValue == MatchType)
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(
					-1,
					15.0f,
					FColor::Cyan,
					FString::Printf(TEXT("Found Session:\n\nID: %s\nUsername: %s\nMatchType: %s"), *Id, *User, *MatchType));
			}

			Subsystem->JoinSession(Result);
		}
	}
}

void UMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	if (!SessionInterface.IsValid())
	{
		return;
	}
	FString Address;
	if (SessionInterface->GetResolvedConnectString(NAME_GameSession, Address))
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.0f,
				FColor::Yellow,
				FString::Printf(TEXT("Server Address: %s"), *Address));
		}

		APlayerController* PC = GetGameInstance()->GetFirstLocalPlayerController();
		if (PC)
		{
			PC->ClientTravel(Address, TRAVEL_Absolute);
		}
	}
}

void UMenu::OnDestroySession(bool bSuccess)
{
	
}

void UMenu::OnStartSession(bool bSuccess)
{
	
}

void UMenu::HostButtonClicked()
{
	if (Subsystem)
	{
		Subsystem->CreateSession(NumPublicConnections, MatchType);
	}
}

void UMenu::JoinButtonClicked()
{
	if (Subsystem)
	{
		Subsystem->FindSessions(10000);
	}
}

void UMenu::MenuTeardown()
{
	RemoveFromParent();
	
	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* PC = World->GetFirstPlayerController();
		if (PC)
		{
			FInputModeGameOnly GameOnly;
			PC->SetInputMode(GameOnly);
			PC->SetShowMouseCursor(false);
		}
	}
}
