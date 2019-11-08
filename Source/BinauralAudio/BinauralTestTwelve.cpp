// Fill out your copyright notice in the Description page of Project Settings.


#include "BinauralTestTwelve.h"

// Sets default values
ABinauralTestTwelve::ABinauralTestTwelve()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	AudioPlayer = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio Player"));
	AudioPlayer->SetupAttachment(this->RootComponent);
	
	// Sets the recommended default values for the binaural simulation
	SoundAttenuation.bAttenuateWithLPF = true;
	SoundAttenuation.bEnableOcclusion = true;
	SoundAttenuation.bEnableLogFrequencyScaling = true;
	SoundAttenuation.SpatializationAlgorithm = ESoundSpatializationAlgorithm::SPATIALIZATION_HRTF;
	SoundAttenuation.StereoSpread = 50.f;
	SoundAttenuation.LPFRadiusMin = 0.f;
	SoundAttenuation.LPFFrequencyAtMin = 20000.f;
	SoundAttenuation.LPFFrequencyAtMax = 20000.f;
	SoundAttenuation.HPFFrequencyAtMax = 5000.f;
	SoundAttenuation.OcclusionVolumeAttenuation = 0.95f;
	SoundAttenuation.OcclusionInterpolationTime = 0.05f;
	//SoundAttenuation.ReverbWetLevelMin = 0.03f;
	//SoundAttenuation.ReverbWetLevelMax = 0.523f;
}

// Called when the game starts or when spawned
void ABinauralTestTwelve::BeginPlay()
{
	Super::BeginPlay();

	// Applies the attenuation settins to the audio file
	if(Audio)
	{
		// This keeps the occlusion frequency filter to a max of the default max minus the highest 
		SoundAttenuation.OcclusionLowPassFilterFrequency = FMath::Clamp(SoundAttenuation.LPFFrequencyAtMax - 3317.f, 0, SoundAttenuation.LPFFrequencyAtMax);
		
		USoundAttenuation* AudioAttenSettings = NewObject<USoundAttenuation>(this);
		AudioAttenSettings->Attenuation = SoundAttenuation;
		Audio->AttenuationSettings = AudioAttenSettings;
	}

	AudioPlayer->SetSound(Audio);
	
	// Dictates whether the sound plays multiple times or just once
	if (PlayOnceOrLoop == EPlayStyle::Loop)
	{
		FTimerHandle PlaySoundTimer;
		FTimerDelegate PlaySoundDelegate;
		PlaySoundDelegate.BindUFunction(this, FName("PlaySound"));
		GetWorld()->GetTimerManager().SetTimer(PlaySoundTimer, PlaySoundDelegate, 1.f, true);
	}else PlaySound();
}

// Called every frame
void ABinauralTestTwelve::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Keeps the maximum absorption and reverb levels at the object's range, and the lower between 0 and 1400
	SoundAttenuation.LPFRadiusMax = GetRange();
	SoundAttenuation.ReverbDistanceMin = FMath::Clamp(GetRange(), 0.f, 1400.f);
	SoundAttenuation.ReverbDistanceMax = GetRange();
}

// Calculates Range between audio source and player
float ABinauralTestTwelve::GetRange()
{
	Range = FVector::Dist(this->GetActorLocation(), PlayerReference->GetActorLocation());
	return Range;
}

// Calculates Elevation of audio source relative to player
float ABinauralTestTwelve::GetElevation()
{
	Elevation = FMath::Sin((this->GetActorLocation().Z - PlayerReference->GetTransform().GetLocation().Z) / Range);
	return Elevation;
}

// Calculates Azimuth of audio source around player
float ABinauralTestTwelve::GetAzimuth()
{
	FVector ForwardPoint = PlayerReference->GetActorForwardVector();
	ForwardPoint.Z = 0;
	ForwardPoint.Normalize();
	FVector ThisLoc = this->GetActorLocation() - PlayerReference->GetActorLocation();
	ThisLoc.Z = 0;
	ThisLoc.Normalize();
	float Dot = FVector::DotProduct(ThisLoc, ForwardPoint);
	Azimuth = UKismetMathLibrary::Acos(Dot) / (PI / 180);

	// Gets the closest ear and sets full 360� rotation
	FVector RightPoint = PlayerReference->GetActorRightVector();
	RightPoint.Z = 0;
	RightPoint.Normalize();
	if (FVector::DotProduct(RightPoint, ThisLoc) > 0)
		Azimuth = 360 - Azimuth;

	return Azimuth;
}

// Plays the newly attenuated sound
void ABinauralTestTwelve::PlaySound()
{
	AudioPlayer->Play();
}
