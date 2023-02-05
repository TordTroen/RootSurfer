// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "RootSurferCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UAnimMontage;
class USoundBase;

UCLASS(config=Game)
class ARootSurferCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* CrouchAction;
	

public:
	ARootSurferCharacter();

protected:
	virtual void BeginPlay();
	virtual void Tick(float DeltaTime) override;

public:
	virtual void Jump() override;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* PrimaryAction;

	/** Bool for AnimBP to switch to another animation set */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon)
	bool bHasRifle;

	/** Setter to set the bool */
	UFUNCTION(BlueprintCallable, Category = Weapon)
	void SetHasRifle(bool bNewHasRifle);

	/** Getter for the bool */
	UFUNCTION(BlueprintCallable, Category = Weapon)
	bool GetHasRifle();

	UPROPERTY(EditDefaultsOnly, Category = "Surf")
	double m_MaxFov = 120.f;

	UPROPERTY(EditDefaultsOnly, Category = "Surf")
	double m_MinFov = 80.f;

	UPROPERTY(EditDefaultsOnly, Category = "Surf")
	double m_FovChangeSpeed = 50.f;

	UPROPERTY(EditDefaultsOnly, Category = "Surf")
	double m_LateralMovementSpeedModifier = 0.8f;

	UPROPERTY(EditDefaultsOnly, Category = "Surf")
	double m_PrimaryActionRange = 10000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Surf")
	double m_GrappleForce = 10000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Surf")
	double m_TraceStartOffset = 90.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Surf")
	TSubclassOf<USceneComponent> m_GrappleHitComponentClass;

private:
	// Divide current speed by this to get the fov we want
	double m_SpeedToFovRatio = 16.f;

	FHitResult m_GrappleHit;

	void UpdateGrapple();
	void StopGrapple();
	void AttachGrapple(FHitResult Hit);
protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	void OnPressCrouch();
	void StopCrouching();

	void DoPrimaryAction();
	void StopPrimaryAction();

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	UPROPERTY(EditDefaultsOnly, Category = "Surf")
	class UCableComponent* m_CableComponent;

	UFUNCTION(BlueprintImplementableEvent)
	void OnGrappleHit(FVector Location);

	UFUNCTION(BlueprintImplementableEvent)
	void OnBeginSlide();

	UFUNCTION(BlueprintImplementableEvent)
	void OnLand();

	UFUNCTION(BlueprintImplementableEvent)
	void OnJump();

	UFUNCTION(BlueprintImplementableEvent)
		void OnPrimaryAction();

	int32 m_SpeedLevel = 0;

	UPROPERTY(EditDefaultsOnly, Category = "Surf")
	TArray<double> m_SpeedLevels;

	UFUNCTION(BlueprintImplementableEvent)
	void OnSpeedLevelChange(int32 SpeedLevel);
};

