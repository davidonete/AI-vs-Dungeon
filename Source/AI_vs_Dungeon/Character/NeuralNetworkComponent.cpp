
#include "AI_vs_Dungeon.h"
#include "Runtime/Engine/Public/Engine.h"
#include "NeuralNetworkComponent.h"

// Sets default values for this component's properties
UNeuralNetworkComponent::UNeuralNetworkComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;

    mRecentAverageError = 1.0;
    mRecentAverageSmoothingFactor = 1.0;
}

// Called when the game starts
void UNeuralNetworkComponent::BeginPlay()
{
	Super::BeginPlay();

    InitializeNetwork();
}

void UNeuralNetworkComponent::InitializeNetwork()
{
    if (mLayers.Num() <= 0)
    {
        //Create layers
        uint16 numLayers = NetworkTopology.Num();
        for (uint16 layerIdx = 0; layerIdx < numLayers; layerIdx++)
        {
            mLayers.Add(Layer());

            //if the layer is the last one there are no outputs
            //if not the number of outputs is the number of neurons of the next layer.
            int32 numOutputs;
            if (layerIdx == numLayers - 1)
                numOutputs = 0;
            else
                numOutputs = NetworkTopology[layerIdx + 1];

            //Fill the layer with neurons (<= we are addin an extra neuron with a constant value (Bias))
            uint16 numNeurons = NetworkTopology[layerIdx];
            for (uint16 neuronIdx = 0; neuronIdx <= numNeurons; neuronIdx++)
            {
                //Add the new neurons to the layer created before
                mLayers.Last().Add(Neuron(numOutputs, neuronIdx));
            }

            //Force the bias node's output value to 1.0
            mLayers.Last().Last().SetOutputValue(1.0);
        }

        //Set default input values
        for (uint16 i = 0; i < NetworkTopology[0]; i++)
            mInputValues.Add(0.0);
    }
}

// Called every frame
void UNeuralNetworkComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );
}

void UNeuralNetworkComponent::FeedForward(const TArray<double> &inputValues)
{
    check(inputValues.Num() == mLayers[0].Num() - 1);

    //Set the output values of the first layer(input layer) as the input values they receive
    //Because the input layer does not modify this values at all.
    for (uint16 i = 0; i < inputValues.Num(); i++)
        mLayers[0][i].SetOutputValue(inputValues[i]);

    //Forward propagate
    //Go over each neuron of the network excluding the last neuron of each layer (the bias neuron)
    for (uint16 layerIdx = 1; layerIdx < mLayers.Num(); layerIdx++)
    {
        Layer &prevLayer = mLayers[layerIdx - 1];
        for (uint16 neuronIdx = 0; neuronIdx < mLayers[layerIdx].Num() - 1; neuronIdx++)
            mLayers[layerIdx][neuronIdx].FeedForward(prevLayer);
    }
}

void UNeuralNetworkComponent::BackPropagate(const TArray<double> &targetValues)
{
    //Calculate overall net error (Root mean square error(RMS) of output network errors)
    Layer &outputLayer = mLayers.Last();
    mError = 0.0;

    //RMS = sqrt((sum((target value - actual value)^2)) / quantity of values - 1)
    for (uint16 neuronIdx = 0; neuronIdx < outputLayer.Num() - 1; neuronIdx++)
    {
        double delta = targetValues[neuronIdx] - outputLayer[neuronIdx].GetOutputValue();
        mError += delta * delta;
    }
    //Get average error squared (not including bias)
    mError /= outputLayer.Num() - 1;
    //Get RMS
    mError = sqrt(mError);

    //Recent average measurement
    mRecentAverageError = (mRecentAverageError * mRecentAverageSmoothingFactor + mError) /
        (mRecentAverageSmoothingFactor + 1.0);

    //Calculate output layer gradients
    for (uint16 neuronIdx = 0; neuronIdx < outputLayer.Num() - 1; neuronIdx++)
        outputLayer[neuronIdx].CalculateOutputGradients(targetValues[neuronIdx]);

    //Calculate hidden layers gradients (from back to front)
    for (uint16 layerIdx = (uint16)mLayers.Num() - 2; layerIdx > 0; layerIdx--)
    {
        Layer &hiddenLayer = mLayers[layerIdx];
        Layer &nextLayer = mLayers[layerIdx + 1];

        for (uint16 neuronIdx = 0; neuronIdx < hiddenLayer.Num(); neuronIdx++)
            hiddenLayer[neuronIdx].CalculateHiddenGradients(nextLayer);
    }

    //Update connection weights for all layers except the input layer (from back to front)
    for (uint16 layerIdx = (uint16)mLayers.Num() - 1; layerIdx > 0; layerIdx--)
    {
        Layer &layer = mLayers[layerIdx];
        Layer &prevLayer = mLayers[layerIdx - 1];

        for (uint16 neuronIdx = 0; neuronIdx < layer.Num() - 1; neuronIdx++)
            layer[neuronIdx].UpdateInputWeights(prevLayer);
    }
}

void UNeuralNetworkComponent::GetResults(TArray<double> &resultValues) const
{
    resultValues.Empty();

    //Get the output values of the last layer (output layer) and save them on the result vals vector
    for (uint16 neuronIdx = 0; neuronIdx < mLayers.Last().Num() - 1; neuronIdx++)
        resultValues.Add(mLayers.Last()[neuronIdx].GetOutputValue());
}

void UNeuralNetworkComponent::GetOutputValues(TArray<double>& w) const
{
    w.Empty();

    //From the fist layer to the last hidden layer (except output layer)
    for (int32 layerIdx = 0; layerIdx < mLayers.Num(); layerIdx++)
    {
        const Layer &layer = mLayers[layerIdx];
        //From the first neuron of the layer to the last (except bias one)
        for (int32 neuronIdx = 0; neuronIdx < layer.Num() - 1; neuronIdx++)
            w.Add(layer[neuronIdx].GetOutputValue());
    }
}

void UNeuralNetworkComponent::SetConnectionWeights(const TArray<double> &w)
{
    unsigned connectionIdx = 0;
    //From the fist layer to the last hidden layer (except output layer)
    for (int32 layerIdx = 0; layerIdx < mLayers.Num() - 1; layerIdx++)
    {
        Layer &layer = mLayers[layerIdx];
        //From the first neuron of the layer to the last
        for (int32 neuronIdx = 0; neuronIdx < layer.Num(); neuronIdx++)
        {
            TArray<Connection> newConnections;
            int32 numConnections = (int32)layer[neuronIdx].GetOutputWeights().Num();
            for (int32 weigthIdx = 0; weigthIdx < numConnections; weigthIdx++)
            {
                newConnections.Add(Connection(w[connectionIdx]));
                connectionIdx++;
            }
            layer[neuronIdx].SetOutputWeights(newConnections);
        }
    }
}

void UNeuralNetworkComponent::GetConnectionWeights(TArray<double> &w)
{
    w.Empty();

    //From the fist layer to the last hidden layer (except output layer)
    for (int32 layerIdx = 0; layerIdx < mLayers.Num() - 1; layerIdx++)
    {
        Layer &layer = mLayers[layerIdx];
        //From the first neuron of the layer to the last
        for (int32 neuronIdx = 0; neuronIdx < layer.Num(); neuronIdx++)
        {
            TArray<Connection> weights = layer[neuronIdx].GetOutputWeights();
            for (int32 weigthIdx = 0; weigthIdx < weights.Num(); weigthIdx++)
                w.Add(weights[weigthIdx].Weight);
        }
    }
}

void UNeuralNetworkComponent::GetConnectionWeightsNoBIAS(TArray<double> &w)
{
    w.Empty();

    //From the fist layer to the last hidden layer (except output layer)
    for (int32 layerIdx = 0; layerIdx < mLayers.Num() - 1; layerIdx++)
    {
        Layer &layer = mLayers[layerIdx];
        //From the first neuron of the layer to the last (except BIAS)
        for (int32 neuronIdx = 0; neuronIdx < layer.Num() - 1; neuronIdx++)
        {
            TArray<Connection> weights = layer[neuronIdx].GetOutputWeights();
            for (int32 weigthIdx = 0; weigthIdx < weights.Num(); weigthIdx++)
                w.Add(weights[weigthIdx].Weight);
        }
    }
}

void UNeuralNetworkComponent::SetInputValue(uint16 index, double value)
{
    if (index < mInputValues.Num())
        mInputValues[index] = value;
}

TArray<double> UNeuralNetworkComponent::GetTargetValues(const TArray<double> &inputValues) const
{
    TArray<double> targetValues;
    //targetValues.Add(ShouldUp(inputValues));
    //targetValues.Add(ShouldLeft(inputValues));
    //targetValues.Add(ShouldRight(inputValues));

    return targetValues;
}

void UNeuralNetworkComponent::PrintArray(FString text, TArray<double> a)
{
    FString arrayText;
    for (uint16 i = 0; i < a.Num(); i++)
        arrayText.Append(FString::Printf(TEXT(" %f,"), a[i]));

    text.Append(arrayText);
    //GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Red, FString::Printf(TEXT("Average Error: %f"), GetRecentAverageError()));
    UE_LOG(LogTemp, Warning, TEXT("%s"), *text);
}

Neuron::Neuron(const uint16 numOutputs, const uint16 idx)
{
    //Save the neuron index inside the layer for doing previous FeedForward.
    mIdx = idx;

    for (uint16 connection = 0; connection < numOutputs; connection++)
    {
        //Create a connection for each output neuron and set a random weight
        mOutputWeights.Add(Connection(RandomWeight()));
    }
}

void Neuron::FeedForward(const Layer &prevLayer)
{
    double sum = 0.0;

    //Sum the previous layer's outputs (which are our inputs)
    //Include the bias neuron from the previous layer
    for (uint16 neuronIdx = 0; neuronIdx < prevLayer.Num(); neuronIdx++)
    {
        //F = Sum (neuron input value * neuron weight)
        sum += prevLayer[neuronIdx].GetOutputValue() *
            prevLayer[neuronIdx].mOutputWeights[mIdx].Weight;
    }

    mOutputVal = Neuron::TransferFunction(sum);
}

void Neuron::CalculateOutputGradients(const double targetValue)
{
    double delta = targetValue - mOutputVal;
    mGradient = delta * Neuron::TransferFunctionDerivative(mOutputVal);
}

void Neuron::CalculateHiddenGradients(const Layer &nextLayer)
{
    double dow = SumDOW(nextLayer);
    mGradient = dow * Neuron::TransferFunctionDerivative(mOutputVal);
}

void Neuron::UpdateInputWeights(Layer &prevLayer)
{
    //Update the connection weights between this neuron and the previous layer
    for (uint16 neuronIdx = 0; neuronIdx < prevLayer.Num(); neuronIdx++)
    {
        Neuron &neuron = prevLayer[neuronIdx];
        double oldDeltaWeight = neuron.mOutputWeights[mIdx].DeltaWeight;

        //The output value of the previous neurons magnified by the gradient and the learning rate
        //And the momentum will determine what percentage of the previous delta weight is maintained
        double newDeltaWeight = mLearningRate * neuron.GetOutputValue() * mGradient +
            mMomentum * oldDeltaWeight;

        neuron.mOutputWeights[mIdx].DeltaWeight = newDeltaWeight;
        neuron.mOutputWeights[mIdx].Weight += newDeltaWeight;
    }
}

double Neuron::TransferFunction(const double x)
{
    return tanh(x);
}

double Neuron::TransferFunctionDerivative(const double x)
{
    return 1.0 - x * x;
}

double Neuron::SumDOW(const Layer &nextLayer) const
{
    double sum = 0.0;

    //Sum our contribution of the errors at the nodes we feed
    for (uint16 neuronIdx = 0; neuronIdx < nextLayer.Num() - 1; neuronIdx++)
        sum += mOutputWeights[neuronIdx].Weight * nextLayer[neuronIdx].mGradient;

    return sum;
}