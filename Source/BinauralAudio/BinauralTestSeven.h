// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Audio.h"
#include "Engine.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Character.h"
#include "AudioMixerDevice.h"
#include "BinauralTestSeven.generated.h"

// Enum that stores the ear closest to the audio source
UENUM()
enum class ECloserEar : uint8 {
	LeftEar UMETA(DisplayName = "Left Ear"),
	RightEar UMETA(DisplayName = "RightEar"),
	EitherEar UMETA(DisplayName = "Either Ear")
};

UCLASS()
class BINAURALAUDIO_API ABinauralTestSeven : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABinauralTestSeven();


	// Audio component that plays the created audio
	UPROPERTY()
		UAudioComponent* AudioPlayer;

	// Audio to make binaural
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
		USoundWave* Audio;

	// Global positioning variables
	UPROPERTY()
		float Azimuth = 0;
	UPROPERTY()
		float Elevation = 0;
	UPROPERTY()
		float Range = 0;
	UPROPERTY()
		ECloserEar CloserEar = ECloserEar::EitherEar;

	// The Base values for the sound modifications
	UPROPERTY()
		float BasePitch = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Volume = 1;
	UPROPERTY()
		TArray<float> DelayArray;
	UPROPERTY()
		TArray<float> PitchArray;
	UPROPERTY()
		TArray<float> VolumeArray;

	// Reference to the listener
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
		ACharacter* PlayerReference;

	// Time at which the sound needs to continue playing after parameters update
	UPROPERTY()
		float PickUpTime = 0;
	bool SoundPlaying = false;

	// Mixer that hold the sound spatialisation settings
	Audio::FMixerDevice MixerDevice;

	// Buffer for output sound
	Audio::AlignedFloatBuffer Buffer;
	
	// Wave instance to play
	FWaveInstance* OutputSound;

	// Final audio source to play
	FSoundSource* SoundSource;
	
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

	// Creates the binaural sound out of calculated values and sound waves
	UFUNCTION()
		void CreateSound();

	// Plays the new binaural sound
	UFUNCTION(BlueprintCallable, Category = "Audio")
		void PlaySound();

};
