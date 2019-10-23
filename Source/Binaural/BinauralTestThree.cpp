// Fill out your copyright notice in the Description page of Project Settings.


#include "BinauralTestThree.h"

// Sets default values
ABinauralTestThree::ABinauralTestThree()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	AudioPlayer = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio Player"));
	AudioPlayer->SetupAttachment(this->RootComponent);
	AudioPlayerRight = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio Player Right"));
	AudioPlayerRight->SetupAttachment(this->RootComponent);
/*
	float Up = 0;
	for (int32 AzimuthCount = 0; AzimuthCount < 90; AzimuthCount++)
	{
		DelayArray[AzimuthCount] = Up;
		DelayArray[AzimuthCount + 90] = 0.000625 - Up;
		DelayArray[AzimuthCount + 180] = Up;
		DelayArray[AzimuthCount + 270] = 0.000625 - Up;
		Up += 0.000625 / 90;
	}*/
}

// Called when the game starts or when spawned
void ABinauralTestThree::BeginPlay()
{
	Super::BeginPlay();
	/*
	if (Audio) RightAudio = Audio;
	Audio->ChannelOffsets.Empty();
	RightAudio->ChannelOffsets.Empty();
	Audio->ChannelOffsets.Add(2);
	RightAudio->ChannelOffsets.Add(3);
	AudioPlayer->SetSound(Audio);
	AudioPlayerRight->SetSound(RightAudio);

	for(int32 i = 0; i < 360; i++)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15, FColor::Yellow, FString::SanitizeFloat(DelayArray[i]));
	}*/
}

// Called every frame
void ABinauralTestThree::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	//test print of variables
	FVector ThisLocation = this->GetActorLocation() - PlayerReference->GetTransform().GetLocation();
	ThisLocation.Normalize();
	FVector Output;
	Output = FVector(GetElevation(), GetAzimuth(), GetRange());
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, Output.ToString());

	AttenuationDetails.DistanceAlgorithm = EAttenuationDistanceModel::Linear;
	AttenuationDetails.AttenuationEval(GetRange(), 1, 1);
}

// Calculates Range between audio source and player
float ABinauralTestThree::GetRange()
{
	Range = FVector::Dist(this->GetActorLocation(), PlayerReference->GetActorLocation());
	return Range;
}
// Calculates Elevation of audio source relative to player
float ABinauralTestThree::GetElevation()
{
	Elevation = FMath::Sin((this->GetActorLocation().Z - PlayerReference->GetTransform().GetLocation().Z) / Range);
	return Elevation;
}
// Calculates Azimuth of audio source around player
float ABinauralTestThree::GetAzimuth()
{
	FVector ForwardPoint = PlayerReference->GetActorForwardVector();
	ForwardPoint.Z = 0;
	ForwardPoint.Normalize();
	FVector ThisLoc = this->GetActorLocation() - PlayerReference->GetActorLocation();
	ThisLoc.Z = 0;
	ThisLoc.Normalize();
	float Dot = FVector::DotProduct(ThisLoc, ForwardPoint);
	Azimuth = UKismetMathLibrary::Acos(Dot) / (PI / 180);
	FVector RightPoint = PlayerReference->GetActorRightVector();
	RightPoint.Z = 0;
	RightPoint.Normalize();
	if (FVector::DotProduct(RightPoint, ThisLoc) > 0) Azimuth = 360 - Azimuth;

	return Azimuth;

}

// Called when the audio is ready to be played
void ABinauralTestThree::PlayFirstEar(ECloserEar FirstEar)
{
	//EarDelegate.BindUFunction(this, FName("PlaySecondEar"), CloserEar);
	//GetWorld()->GetTimerManager().SetTimer(EarTimer, EarDelegate, DelayArray[FMath::RoundFromZero(Azimuth)], false);
	if(FirstEar == ECloserEar::RightEar) AudioPlayerRight->Play();
	else AudioPlayer->Play();
}
void ABinauralTestThree::PlaySecondEar(ECloserEar SecondEar)
{
	AudioPlayerRight->Play();
	if (SecondEar == ECloserEar::RightEar) AudioPlayer->Play();
	else AudioPlayerRight->Play();
}