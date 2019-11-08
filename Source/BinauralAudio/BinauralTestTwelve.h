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

// Structs that hold the minimum and maximum frequencies of HRTF audio inputs
USTRUCT(BlueprintType)
struct FMaximumFrequencies
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Minus20Degrees;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Minus10Degrees;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float At0Degrees;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Positive10Degrees;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Positive20Degrees;

	FMaximumFrequencies()
	{
		Minus20Degrees = 2974;
		Minus10Degrees = 2972;
		At0Degrees = 3053;
		Positive10Degrees = 3317;
		Positive20Degrees = 3230;
	}

};
USTRUCT(BlueprintType)
struct FMinimumFrequencies
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Minus20Degrees;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Minus10Degrees;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float At0Degrees;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Positive10Degrees;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Positive20Degrees;

	FMinimumFrequencies()
	{
		Minus20Degrees = 2256;
		Minus10Degrees = 2287;
		At0Degrees = 2154;
		Positive10Degrees = 2138;
		Positive20Degrees = 2124;
	}

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

	// Spatialisation settings for the audio
	UPROPERTY()
		FSoundAttenuationSettings SoundAttenuation;

	// Reference to the listener
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
		AActor* PlayerReference;

	// Structs to pull the frequency offsets from
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
		FMaximumFrequencies MaxFrequencies;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
		FMinimumFrequencies MinFrequencies;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Simulates head occlusion
	float GetOcclusionFrequency();
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
