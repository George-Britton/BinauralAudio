// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine.h"
#include "AudioAnalyser.generated.h"

UCLASS()
class BINAURALAUDIO_API AAudioAnalyser : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAudioAnalyser();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USoundWave* Audio;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
