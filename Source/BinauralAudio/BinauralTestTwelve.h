// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Audio.h"
#include "Engine.h"
#include "Kismet\KismetMathLibrary.h"
#include "Runtime\Engine\Classes\Sound\SoundAttenuation.h"
#include "BinauralTestTwelve.generated.h"

// Enum that stores the ear closest to the audio source
UENUM()
enum class EPlayStyle : uint8 {
	Loop UMETA(DisplayName = "Loop"),
	Once UMETA(DisplayName = "Once"),
	MAX UMETA(DisplayName = "Max")
};

// Enum that stores the ear closest to the audio source
UENUM()
enum class ECloserEar : uint8 {
	LeftEar UMETA(DisplayName = "Left Ear"),
	RightEar UMETA(DisplayName = "RightEar"),
	EitherEar UMETA(DisplayName = "Either Ear"),
	MAX UMETA(DisplayName = "MAX")
};

UCLASS()
class BINAURALAUDIO_API ABinauralTestTwelve : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABinauralTestTwelve();

	// Audio component that plays the created audio
	UPROPERTY()
		UAudioComponent* AudioPlayer;

	// Audio to make binaural
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
		USoundWave* Audio;
	
	// Whether the audio needs to be looped or played just once
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
		EPlayStyle PlayOnceOrLoop = EPlayStyle::Once;

	// Reference to the listener
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
		AActor* PlayerReference;

	// Spatialisation settings for the audio
	UPROPERTY(EditAnywhere)
		FSoundAttenuationSettings SoundAttenuation;

	// Global positioning variables
	UPROPERTY()
		float Azimuth = 0;
	UPROPERTY()
		float Elevation = 0;
	UPROPERTY()
		float Range = 0;
	UPROPERTY()
		ECloserEar CloserEar = ECloserEar::EitherEar;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Calculates Range between audio source and player
	UFUNCTION()
		float GetRange();
	// Calculates Elevation of audio source relative to player
	UFUNCTION()
		float GetElevation();
	// Calculates Azimuth of audio source around player
	UFUNCTION()
		float GetAzimuth();

	// Plays the newly attenuated sound
	UFUNCTION()
		void PlaySound();
};
