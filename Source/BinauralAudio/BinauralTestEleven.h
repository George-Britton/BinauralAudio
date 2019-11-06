// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Audio.h"
#include "Engine.h"
#include "Kismet\KismetMathLibrary.h"
#include "GameFramework\Character.h"
#include "AudioMixerDevice.h"
#include "AudioMixer.h"
#include "Private/AudioMixerPlatformXAudio2.h"
#include "BinauralTestEleven.generated.h"

struct FWaveInstance;

// Enum that stores the three critical data points
UENUM()
enum class EHRTFPoint : uint8 {
	Delay UMETA(DisplayName = "Delay"),
	Pitch UMETA(DisplayName = "Pitch"),
	Volume UMETA(DisplayName = "Volume"),
	MAX UMETA(DisplayName = "MAX")
};
// Enum that stores the ear closest to the audio source
UENUM()
enum class ECloserEar : uint8 {
	LeftEar UMETA(DisplayName = "Left Ear"),
	RightEar UMETA(DisplayName = "RightEar"),
	EitherEar UMETA(DisplayName = "Either Ear"),
	MAX UMETA(DisplayName = "MAX")
};
// Structs holding the maximum offsets for the HRTF measurements
USTRUCT(BlueprintType)
struct FMaximumDelayOffsets
{
	GENERATED_BODY()
	UPROPERTY()
		float Minus20Degrees;
	UPROPERTY()
		float Minus10Degrees;
	UPROPERTY()
		float At0Degrees;
	UPROPERTY()
		float Positive10Degrees;
	UPROPERTY()
		float Positive20Degrees;
	FMaximumDelayOffsets()
	{
		Minus20Degrees = 0.00061;
		Minus10Degrees = 0.00064;
		At0Degrees = 0.00068;
		Positive10Degrees = 0.0006;
		Positive20Degrees = 0.00059;
	}
};
USTRUCT(BlueprintType)
struct FMaximumPitchOffsets
{
	GENERATED_BODY()
	UPROPERTY()
		float Minus20Degrees;
	UPROPERTY()
		float Minus10Degrees;
	UPROPERTY()
		float At0Degrees;
	UPROPERTY()
		float Positive10Degrees;
	UPROPERTY()
		float Positive20Degrees;
	FMaximumPitchOffsets()
	{
		Minus20Degrees = 0.0963;
		Minus10Degrees = 0.0501;
		At0Degrees = 0.0523;
		Positive10Degrees = 0.0495;
		Positive20Degrees = 0.0768;
	}
};
USTRUCT(BlueprintType)
struct FMaximumVolumeOffsets
{
	GENERATED_BODY()
	UPROPERTY()
		float Minus20Degrees;
	UPROPERTY()
		float Minus10Degrees;
	UPROPERTY()
		float At0Degrees;
	UPROPERTY()
		float Positive10Degrees;
	UPROPERTY()
		float Positive20Degrees;
	FMaximumVolumeOffsets()
	{
		Minus20Degrees = 0.11;
		Minus10Degrees = 0.2195;
		At0Degrees = 0.2381;
		Positive10Degrees = 0.2458;
		Positive20Degrees = 0.1955;
	}
};
// Struct holding all the maximum HRTF offsets
USTRUCT(BlueprintType)
struct FHRTFMaximums
{
	GENERATED_BODY()
	UPROPERTY()
		FMaximumDelayOffsets MaximumDelayOffsets;
	UPROPERTY()
		FMaximumPitchOffsets MaximumPitchOffsets;
	UPROPERTY()
		FMaximumVolumeOffsets MaximumVolumeOffsets;
	FHRTFMaximums()
	{
		FMaximumDelayOffsets MaximumDelayOffsets;
		FMaximumPitchOffsets MaximumPitchOffsets;
		FMaximumVolumeOffsets MaximumVolumeOffsets;
	}
};

// Struct holding the HRTF values for the synthesizer
USTRUCT(BlueprintType)
struct FHRTFValuesBasedOnElevation
{
	GENERATED_BODY()
	UPROPERTY()
		float DelayOffset;
	UPROPERTY()
		float PitchOffset;
	UPROPERTY()
		float VolumeOffset;
	FHRTFValuesBasedOnElevation()
	{
		DelayOffset = 0;
		PitchOffset = 0;
		VolumeOffset = 0;
	}
};

// Struct that holds the binaural synthesizing variables for each ausio source in the array
USTRUCT(BlueprintType)
struct FBinauralSynthesizer
{
	GENERATED_BODY()

		// Audio to make binaural
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
			USoundWave* Audio;

		// Actor "emitting" the sound
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
			AActor* AudioSource;

		// Reference to the listener
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
			ACharacter* PlayerReference;

		// Audio component that plays the created audio
		UPROPERTY()
			UAudioComponent* AudioPlayer;

		// Array of HRTF measurements
		UPROPERTY()
			FHRTFMaximums HRTFMaximums;

		// The Base values for the sound modifications
		UPROPERTY()
			float BasePitch = 1;
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			float Volume = 1;

		// Global positioning variables
		UPROPERTY()
			float Azimuth = 0;
		UPROPERTY()
			float Elevation = 0;
		UPROPERTY()
			float Range = 0;
		UPROPERTY()
			ECloserEar CloserEar = ECloserEar::EitherEar;

		// Time at which the sound needs to continue playing after parameters update
		UPROPERTY()
			float PickUpTime = 0;
		bool SoundPlaying = false;

		// Mixer that hold the sound spatialisation settings
		Audio::FMixerDevice* MixerDevice;

		// Buffer for output sound
		Audio::AlignedFloatBuffer Buffer;

		// Active sound used for making the WaveInstance
		FActiveSound ActiveSound;

		// Wave instances to play
		FWaveInstance* LeftSoundPtr;
		FWaveInstance* RightSoundPtr;

		// Final audio source to play
		FSoundSource* SoundSource;

};

// Class that holds the binaural synthesizer
UCLASS()
class BINAURALAUDIO_API ABinauralTestEleven : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABinauralTestEleven();

	// Array of actors and sounds to synthesize binaural sounds for
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Synthesizer Array")
		TArray<FBinauralSynthesizer> BinauralSynthesizer;

	// Arrays of HRTF measurements based on elevation
	UPROPERTY()
		TArray<FHRTFValuesBasedOnElevation> HRTF_Minus20Degrees;
	UPROPERTY()
		TArray<FHRTFValuesBasedOnElevation> HRTF_Minus10Degrees;
	UPROPERTY()
		TArray<FHRTFValuesBasedOnElevation> HRTF_0Degrees;
	UPROPERTY()
		TArray<FHRTFValuesBasedOnElevation> HRTF_Positive10Degrees;
	UPROPERTY()
		TArray<FHRTFValuesBasedOnElevation> HRTF_Positive20Degrees;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when the HRTF arrays are initialised
	void InitialiseHRTFs(TArray<FHRTFValuesBasedOnElevation> HRTFArray, int8 ElevationIncrementer, float MaxDelay, float MaxPitch, float MaxVolume);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Calculates Range between audio source and player
	UFUNCTION()
		float GetRange(int32 Index);
	// Calculates Elevation of audio source relative to player
	UFUNCTION()
		float GetElevation(int32 Index);
	// Calculates Azimuth of audio source around player
	UFUNCTION()
		float GetAzimuth(int32 Index);

	// Creates the binaural sound out of calculated values and sound waves
	UFUNCTION()
		void CreateSound(int32 Index);

	// Sets up the Wave Instance variables
	void WaveInstanceSetup(int32 Index, FWaveInstance* WaveToMod, ECloserEar SideEar);

	// Finds the correct offsets from the arrays
	float FindOffset(int32 Index, EHRTFPoint DataPoint);

	// Plays the new binaural sound
	UFUNCTION(BlueprintCallable, Category = "Audio")
		void PlaySound(int32 Index);

};
