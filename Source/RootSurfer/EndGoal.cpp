// Fill out your copyright notice in the Description page of Project Settings.


#include "EndGoal.h"

// Sets default values
AEndGoal::AEndGoal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEndGoal::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEndGoal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

