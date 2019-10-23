// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"
#include "BinauralTestThree.generated.h"

UENUM()
enum class ECloserEar : uint8 {
	LeftEar UMETA(DisplayName = "Left Ear"),
	RightEar UMETA(DisplayName = "RightEar"),
	EitherEar UMETA(DisplayName = "Either Ear")
};

UCLASS()
class BINAURAL_API ABinauralTestThree : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABinauralTestThree();

	// Audio component that plays the created audio
	UPROPERTY()
		UAudioComponent* AudioPlayer;
	UPROPERTY()
		UAudioComponent* AudioPlayerRight;
	
	// Audio to make binaural
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
		USoundWave* Audio;
	UPROPERTY()
		USoundWave* RightAudio;

	UPROPERTY()
		float Azimuth = 0;
	UPROPERTY()
		float Elevation = 0;
	UPROPERTY()
		float Range = 0;
	UPROPERTY()
		ECloserEar CloserEar = ECloserEar::EitherEar;

	UPROPERTY()
		float BasePitch = 1;
	UPROPERTY()
		float BaseVolume = 1;
	UPROPERTY()
		TArray<float> DelayArray;

	UPROPERTY()
		FSoundAttenuationSettings AttenuationDetails;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
		ACharacter* PlayerReference;

	UPROPERTY()
		FTimerHandle EarTimer;
	//UPROPERTY()
	//	FTimerDelegate EarDelegate;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		float GetAzimuth();
	UFUNCTION()
		float GetElevation();
	UFUNCTION()
		float GetRange();

	UFUNCTION()
		void PlayFirstEar(ECloserEar FirstEar);
	UFUNCTION()
		void PlaySecondEar(ECloserEar SecondEar);
};
