// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine.h"
#include "Kismet/KismetMathLibrary.h"
#include "ActiveSound.h"
#include "UnrealAudioSoundFile.h"
#include "Serialization/BulkData.h"
#include "BinauralTestOne.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BINAURALAUDIO_API UBinauralTestOne : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBinauralTestOne();


	UPROPERTY()
		UAudioComponent* LeftEar;
	UPROPERTY()
		UAudioComponent* RightEar;

	UPROPERTY()
		float Azimuth = 0;
	UPROPERTY()
		float Elevation = 0;
	UPROPERTY()
		float Range = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		USoundWave* Audio;
	UPROPERTY()
		USoundWave* RightAudio;
	UPROPERTY()
		TArray<FSoundWaveSpectralTimeData> DataToAnalyse;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
		ACharacter* PlayerReference;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
		float GetAzimuth();
	UFUNCTION()
		float GetElevation();
	UFUNCTION()
		float GetRange();
	
};
