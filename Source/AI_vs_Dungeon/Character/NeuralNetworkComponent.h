//
//  NeuralNetworkComponent.h
//  AI vs Dungeon
//
//  Created by David Parra (davidparraausina@gmail.com) on 12/03/17.
//  Copyright � 2016 David Parra. All rights reserved.
//

#pragma once

#include "Components/ActorComponent.h"
#include "NeuralNetworkComponent.generated.h"

class Neuron;
typedef TArray<Neuron> Layer;

UENUM(BlueprintType)
enum NNInputType
{
    GroundCollision = 0,
    BackwardCollision,
    UpCollision,
    ForwardCollision
};

struct Connection
{
    double Weight;
    double DeltaWeight;

    Connection(double w) { Weight = w; DeltaWeight = 0.0; }
};

class Neuron
{
public:
    Neuron(const uint16 numOutputs, const uint16 idx);
    ~Neuron() {}

    void FeedForward(const Layer &prevLayer);

    inline void SetOutputValue(const double value) { mOutputVal = value; }
    inline double GetOutputValue() const { return mOutputVal; }
    inline TArray<Connection> GetOutputWeights() { return mOutputWeights; }
    inline void SetOutputWeights(TArray<Connection> weights) { mOutputWeights = weights; }

    void CalculateOutputGradients(const double targetValue);
    void CalculateHiddenGradients(const Layer &nextLayer);
    void UpdateInputWeights(Layer &prevLayer);

private:
    static double TransferFunction(const double x);
    static double TransferFunctionDerivative(const double x);
    static double RandomWeight() { return rand() / double(RAND_MAX); }

    double SumDOW(const Layer &nextLayer) const;

    uint16 mIdx;
    double mGradient;
    double mOutputVal;
    TArray<Connection> mOutputWeights;

    //Overall network learning rate [0.0...1.0]
    double mLearningRate = 0.15;
    //Multiplier of last weight change [0.0...n]
    double mMomentum = 0.5;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AI_VS_DUNGEON_API UNeuralNetworkComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UNeuralNetworkComponent();

	// Called when the game starts
	virtual void BeginPlay() override;

    void InitializeNetwork();
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

    void FeedForward() { FeedForward(mInputValues); };
    void FeedForward(const TArray<double> &inputValues);
    void BackPropagate(const TArray<double> &targetValues);
    void GetResults(TArray<double> &resultValues) const;

    inline double GetRecentAverageError() const { return mRecentAverageError; }

    void SetConnectionWeights(const TArray<double> &w);
    void GetConnectionWeights(TArray<double> &w);

    //down, back, up, forward
    void SetInputValue(uint16 index, double value);
    inline TArray<double> GetInputValues() { return mInputValues; };

    TArray<double> GetTargetValues(const TArray<double> &inputValues) const;

    void PrintArray(FString text, TArray<double> a);
    void PrintInputValues() { PrintArray("In: ", mInputValues); }

private:
    //The actual topology of the neural network ([0] num inputs in layer, [1..n] num hidden neurons in layer, [n+1] num outputs in layer)
    UPROPERTY(EditDefaultsOnly, Category = "Network Configuration")
    TArray<uint16> NetworkTopology;

    TArray<Layer> mLayers;

    //down, back, up, forward
    TArray<double> mInputValues;

    double mError;
    double mRecentAverageError;
    double mRecentAverageSmoothingFactor;
};
