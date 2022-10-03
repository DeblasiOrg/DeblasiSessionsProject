// Fill out your copyright notice in the Description page of Project Settings.


#include "DeblasiSessionsSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "OnlineSubsystem.h"

UDeblasiSessionsSubsystem::UDeblasiSessionsSubsystem()
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem)
	{
		SessionInterface = Subsystem->GetSessionInterface();

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.0f,
				FColor::Purple,
				FString::Printf(TEXT("Found Subsystem %s"), *Subsystem->GetSubsystemName().ToString()));
		}
	}

	CreateSessionCompleteDelegate.BindUObject(this, &UDeblasiSessionsSubsystem::OnCreateSessionComplete);
	FindSessionsCompleteDelegate.BindUObject(this, &UDeblasiSessionsSubsystem::OnFindSessionsComplete);
	JoinSessionCompleteDelegate.BindUObject(this, &UDeblasiSessionsSubsystem::OnJoinSessionComplete);
	DestroySessionCompleteDelegate.BindUObject(this, &UDeblasiSessionsSubsystem::OnDestroySessionComplete);
	StartSessionCompleteDelegate.BindUObject(this, &UDeblasiSessionsSubsystem::OnStartSessionComplete);
}

void UDeblasiSessionsSubsystem::CreateSession(int32 NumPublicConnections, FString MatchType)
{
	if (!SessionInterface.IsValid())
	{
		return;
	}

	if (auto ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession))
	{
		SessionInterface->DestroySession(NAME_GameSession);
		if (GEngine)
		{
			FName SessionName = NAME_GameSession;
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.0f,
				FColor::Red,
				FString::Printf(TEXT("Session with name %s already exists, destroying"), *SessionName.ToString()));
		}
	}

	CreateSessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);
	
	LastSessionSettings = MakeShareable(new FOnlineSessionSettings());
	LastSessionSettings->bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
	LastSessionSettings->NumPublicConnections = NumPublicConnections;
	LastSessionSettings->bAllowJoinInProgress = true;
	LastSessionSettings->bAllowJoinViaPresence = true;
	LastSessionSettings->bShouldAdvertise = true;
	LastSessionSettings->bUsesPresence = true;
	LastSessionSettings->bUseLobbiesIfAvailable = true;
	LastSessionSettings->Set(FName("MatchType"), MatchType, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *LastSessionSettings))
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
		DeblasiSessionsOnCreateSessionComplete.Broadcast(false);
	}
}

void UDeblasiSessionsSubsystem::FindSessions(int32 MaxSearchResults)
{
	if (!SessionInterface.IsValid())
	{
		return;
	}

	FindSessionsCompleteDelegateHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegate);
	
	LastSessionSearch = MakeShareable(new FOnlineSessionSearch());
	LastSessionSearch->MaxSearchResults = 10000;
	LastSessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
	LastSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
	
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!SessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId().GetUniqueNetId(), LastSessionSearch.ToSharedRef()))
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);

		DeblasiSessionsOnFindSessionsComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
	}
}

void UDeblasiSessionsSubsystem::JoinSession(const FOnlineSessionSearchResult& Result)
{
	if (!SessionInterface.IsValid())
	{
		DeblasiSessionsOnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
		return;
	}

	JoinSessionCompleteDelegateHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);
	
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!SessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId().GetUniqueNetId(), NAME_GameSession, Result))
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);

		DeblasiSessionsOnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
	}
}

void UDeblasiSessionsSubsystem::DestroySession()
{
	
}

void UDeblasiSessionsSubsystem::StartSession()
{
	
}



void UDeblasiSessionsSubsystem::OnCreateSessionComplete(FName SessionName, bool bSuccess)
{
	if (SessionInterface)
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
	}
	
	DeblasiSessionsOnCreateSessionComplete.Broadcast(bSuccess);
}

void UDeblasiSessionsSubsystem::OnFindSessionsComplete(bool bSuccess)
{
	if (SessionInterface)
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
	}

	if (LastSessionSearch->SearchResults.Num() <= 0)
	{
		DeblasiSessionsOnFindSessionsComplete.Broadcast(LastSessionSearch->SearchResults, false);
		return;
	}

	DeblasiSessionsOnFindSessionsComplete.Broadcast(LastSessionSearch->SearchResults, bSuccess);
}

void UDeblasiSessionsSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (SessionInterface)
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
	}

	DeblasiSessionsOnJoinSessionComplete.Broadcast(Result);
}

void UDeblasiSessionsSubsystem::OnDestroySessionComplete(FName SessionName, bool bSuccess)
{
	
}

void UDeblasiSessionsSubsystem::OnStartSessionComplete(FName SessionName, bool bSuccess)
{
	
}
