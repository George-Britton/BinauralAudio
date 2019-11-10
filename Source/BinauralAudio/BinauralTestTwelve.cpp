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

	// Sets the values for the non-dynamic sound attenuation variables
	SoundAttenuation.bAttenuateWithLPF = true;
	SoundAttenuation.bEnableOcclusion = true;
	SoundAttenuation.bEnableLogFrequencyScaling = true;
	SoundAttenuation.SpatializationAlgorithm = ESoundSpatializationAlgorithm::SPATIALIZATION_HRTF;
	SoundAttenuation.StereoSpread = 15.3f;
	SoundAttenuation.LPFRadiusMin = 0.f;
	SoundAttenuation.LPFFrequencyAtMin = 20000.f;
	SoundAttenuation.LPFFrequencyAtMax = 20000.f;
	SoundAttenuation.HPFFrequencyAtMax = 2124.f;
	SoundAttenuation.OcclusionVolumeAttenuation = 0.95f;
}

// Called when the game starts or when spawned
void ABinauralTestTwelve::BeginPlay()
{
	Super::BeginPlay();

	// Applies the attenuation settins to the audio file
	if(Audio)
	{
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

// Simulates head occlusion
float ABinauralTestTwelve::GetOcclusionFrequency()
{
	float TestingElevation = GetElevation();

	if (TestingElevation < -15)
	{
		return (MaxFrequencies.Minus20Degrees - MinFrequencies.Minus20Degrees);
	}else if (TestingElevation >= -15 && TestingElevation < -5)
	{
		return (MaxFrequencies.Minus10Degrees - MinFrequencies.Minus10Degrees);
	}else if (TestingElevation >= 5 && TestingElevation <= 5)
	{
		return (MaxFrequencies.At0Degrees - MinFrequencies.At0Degrees);
	}else if (TestingElevation <= 15 && TestingElevation > 5)
	{
		return (MaxFrequencies.Positive10Degrees - MinFrequencies.Positive10Degrees);
	} else if (TestingElevation > 15)
	{
		return (MaxFrequencies.Positive20Degrees - MinFrequencies.Positive20Degrees);
	}

	return 0;
}

// Called every frame
void ABinauralTestTwelve::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Keeps the maximum absorption and reverb levels at the object's range, and the lower between 0 and 1400
	SoundAttenuation.LPFRadiusMax = GetRange();
	SoundAttenuation.ReverbDistanceMin = FMath::Clamp(GetRange(), 0.f, 1400.f);
	SoundAttenuation.ReverbDistanceMax = GetRange();
	
	// This keeps the occlusion frequency filter to a max of the default max minus the highest 
	SoundAttenuation.OcclusionLowPassFilterFrequency = SoundAttenuation.LPFFrequencyAtMax - GetOcclusionFrequency();
	
	SoundAttenuation.OcclusionInterpolationTime = 0.02;
}

// Calculates Range between audio source and player
float ABinauralTestTwelve::GetRange()
{
	float Range = FVector::Dist(this->GetActorLocation(), PlayerReference->GetActorLocation());
	return Range;
}

// Calculates Elevation of audio source relative to player
float ABinauralTestTwelve::GetElevation()
{
	float Elevation = FMath::Sin((this->GetActorLocation().Z - PlayerReference->GetTransform().GetLocation().Z) / GetRange());
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
	float Azimuth = FVector::DotProduct(ThisLoc, ForwardPoint);

	return Azimuth;
}

// Plays the newly attenuated sound
void ABinauralTestTwelve::PlaySound()
{
	AudioPlayer->Play();
}
