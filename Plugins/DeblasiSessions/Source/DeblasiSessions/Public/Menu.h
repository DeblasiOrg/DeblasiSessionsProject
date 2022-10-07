// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DeblasiSessionsSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Menu.generated.h"

/**
 * 
 */
UCLASS()
class DEBLASISESSIONS_API UMenu : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	void MenuSetup(int32 NumberOfPublicConnections = 4, FString TypeOfMatch = TEXT("FreeForAll"));
	
protected:
	
	virtual bool Initialize() override;
	virtual void OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld) override;

	// Callbacks for custom delegates in subsystem
	UFUNCTION()
	void OnCreateSession(bool bSuccess);
	void OnFindSession(const TArray<FOnlineSessionSearchResult>& SearchResults, bool bSuccess);
	void OnJoinSession(EOnJoinSessionCompleteResult::Type Result);
	UFUNCTION()
	void OnDestroySession(bool bSuccess);
	UFUNCTION()
	void OnStartSession(bool bSuccess);
	
private:

	IOnlineSessionPtr SessionInterface;
	
	UPROPERTY(meta = (BindWidget))
	UButton* HostButton;
	UPROPERTY(meta = (BindWidget))
	UButton* JoinButton;

	UFUNCTION()
	void HostButtonClicked();
	UFUNCTION()
	void JoinButtonClicked();

	UPROPERTY()
	UDeblasiSessionsSubsystem* Subsystem;
	UFUNCTION()
	void MenuTeardown();

	int32 NumPublicConnections{4};

	FString MatchType{TEXT("FreeForAll")};

	UFUNCTION()
	void SendVersionCheckRequest();
	void OnVersionCheckResponse(TSharedPtr<FJsonObject> Content, bool bSuccess);

	UPROPERTY(meta=(BindWidget))
	UTextBlock* VersionText;
	
	FString VersionCheckURL = "localhost:3000/version";
	
};
