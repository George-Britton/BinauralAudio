// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "Audio.h"
#include "Engine.h"
#include "Kismet/KismetMathLibrary.h"
#include "Factories/SoundSurroundFactory.h"
#include "BinauralTestFour.generated.h"

// Enum that stores the ear closest to the audio source
UENUM()
enum class ECloserEar : uint8 {
	LeftEar UMETA(DisplayName = "Left Ear"),
	RightEar UMETA(DisplayName = "RightEar"),
	EitherEar UMETA(DisplayName = "Either Ear")
};

UCLASS()
class BINAURALAUDIO_API ABinauralTestFour : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	ABinauralTestFour();

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
	UPROPERTY()
		float BaseVolume = 1;
	UPROPERTY()
		TArray<float> DelayArray;
	
	// Reference to the listener
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
		ACharacter* PlayerReference;

	// Factory variables the create the output sound
	UPROPERTY()
		class USoundSurroundFactory* SoundSurroundFactory;
	UPROPERTY()
		UObject* FactorySound;
	UPROPERTY()
		FFeedbackContext* Warnings;
	
		const uint8* Buffer;
		const uint8* BufferEnd = 16;

	// Sound to be played
	UPROPERTY()
		USoundWave* OutputSound;

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
		void CreateSound();
};
