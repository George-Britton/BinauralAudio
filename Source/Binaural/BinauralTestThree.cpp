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

	DelayArray.Init(0, 361);
	float Up = 0;
	for (int32 AzimuthCount = 0; AzimuthCount < 90; AzimuthCount++)
	{
		DelayArray[AzimuthCount] = Up;
		DelayArray[AzimuthCount + 90] = 0.000625 - Up;
		DelayArray[AzimuthCount + 180] = Up;
		DelayArray[AzimuthCount + 270] = 0.000625 - Up;
		Up += 0.000625 / 90;
	}
}

// Called when the game starts or when spawned
void ABinauralTestThree::BeginPlay()
{
	Super::BeginPlay();
	
	if (Audio)
	{
		if(FCString::Strcmp(*Audio->GetFullName(), TEXT("WAV")) != 0)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Must be .wav filetype");
			PrimaryActorTick.bCanEverTick = false;
		}else
		{
			RightAudio = Audio;
			AudioPlayerRight->SetSound(RightAudio);

			PlayDelegate.BindUFunction(this, "PlayFirstEar");
			GetWorld()->GetTimerManager().SetTimer(PlayTimer, PlayDelegate, 0.5, true);
		}
	}
}

// Called every frame
void ABinauralTestThree::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// Sets base input variables
	Range = GetRange();
	Elevation = GetElevation();
	Azimuth = GetAzimuth();

	//test print of variables
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::SanitizeFloat(DelayArray[FMath::FloorToInt(GetAzimuth())]));
	//if(CloserEar == ECloserEar::LeftEar) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta, "Left Ear");
	//else GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta, "Right Ear");

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

	// Gets the closest ear and sets full 360° rotation
	FVector RightPoint = PlayerReference->GetActorRightVector();
	RightPoint.Z = 0;
	RightPoint.Normalize();
	if (FVector::DotProduct(RightPoint, ThisLoc) > 0)
	{
		Azimuth = 360 - Azimuth;
		CloserEar = ECloserEar::RightEar;
	}else CloserEar = ECloserEar::LeftEar;

	return Azimuth;
}

// Called when the audio is ready to be played
void ABinauralTestThree::PlayFirstEar()
{
	EarDelegate.BindUFunction(this, "PlaySecondEar");
	GetWorld()->GetTimerManager().SetTimer(EarTimer, EarDelegate, DelayArray[FMath::FloorToInt(Azimuth)], false);
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, "First audio");
	if(CloserEar == ECloserEar::RightEar) AudioPlayerRight->Play();
	else AudioPlayer->Play();
}
void ABinauralTestThree::PlaySecondEar()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, "Second audio");
	if (CloserEar == ECloserEar::RightEar) AudioPlayer->Play();
	else AudioPlayerRight->Play();






	FName OutputSoundLeftName = "Output Sound";
	USoundWave* OutputSoundLeft = NewObject<USoundWave>(this, OutputSoundLeftName);
	FName OutputSoundRightName = "Output Sound";
	USoundWave* OutputSoundRight = NewObject<USoundWave>(this, OutputSoundRightName);

	if (OutputSoundLeft->ChannelOffsets.Num())
	{
		OutputSoundLeft->ChannelOffsets.Empty(SPEAKER_Count);
		OutputSoundLeft->ChannelOffsets.AddZeroed(SPEAKER_Count);
	}
	if (OutputSoundLeft->ChannelSizes.Num())
	{
		OutputSoundLeft->ChannelSizes.Empty(SPEAKER_Count);
		OutputSoundLeft->ChannelSizes.AddZeroed(SPEAKER_Count);
	}
	if (OutputSoundRight->ChannelOffsets.Num())
	{
		OutputSoundRight->ChannelOffsets.Empty(SPEAKER_Count);
		OutputSoundRight->ChannelOffsets.AddZeroed(SPEAKER_Count);
	}
	if (OutputSoundRight->ChannelSizes.Num())
	{
		OutputSoundRight->ChannelSizes.Empty(SPEAKER_Count);
		OutputSoundRight->ChannelSizes.AddZeroed(SPEAKER_Count);
	}

	
	



}