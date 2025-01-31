#include<bits/stdc++.h>
#include<cmath>
#include<Eigen/Dense>
#include<cstdio>

typedef Eigen::MatrixXd Matrix;
typedef Eigen::RowVectorXd RowVector;
typedef Eigen::VectorXd ColVector;
typedef long double ldouble;
typedef vector<RowVector> data;
using namespace std;

void ReadCSV(const string& filename, data& dat,bool skipfirstline=false) {
    dat.clear();
    ifstream file(filename);
    cout<<"here"<<endl;
    if (!file.is_open()) {
        cerr << "Error: Could not open file " << filename << endl;
        return;
    }

    string line;
    cout<<"starting to read data"<<endl;
    int tcou=0;
    while (getline(file, line)) {

        if(skipfirstline){
            skipfirstline=0;
            continue;
        }
        //if(tcou%1000==0)cout<<"read "<<tcou<<" lines"<<endl;
        if(tcou==5000)break;
        tcou++;
        stringstream ss(line);
        vector<double> parsed_vec;
        string word;

        while (getline(ss, word, ',')) {
            // Trim whitespace and convert to ldouble.
            try {
                parsed_vec.push_back(stold(word));
            } catch (const invalid_argument& e) {
                cerr << "Error: Invalid number format in line: " << line << endl;
                continue;
            }
        }
        //cout<<parsed_vec.size()<<endl;
        // Create a RowVector from the parsed data.
        RowVector row(parsed_vec.size());
        for (size_t i = 0; i < parsed_vec.size(); ++i) {
            row(i) = parsed_vec[i];
            //if(tcou==1)cout<<row(i);
        }
        dat.push_back(row);
    }
    //cout<<dat[13](75)<<endl;
    cout<<"data read"<<endl;
}

void genData(const string& filename) {
    ofstream file1(filename + "-in");
    ofstream file2(filename + "-out");

    if (!file1.is_open() || !file2.is_open()) {
        cerr << "Error: Could not create output files." << endl;
        return;
    }

    srand(static_cast<unsigned int>(time(0))); // Seed for randomness.

    for (int r = 0; r < 10000; ++r) {
        double x = static_cast<double>(rand()) / RAND_MAX;
        double y = static_cast<double>(rand()) / RAND_MAX;
        file1 << x << "," << y << endl;
        int ans=0;
        if(x>y)ans=1;
        file2 << ans<<","<<(ans+1)%2 << endl; // Generate binary output.
    }

    file1.close();
    file2.close();
}


class NeuralNetwork{
public:
    //int layers;

    // constructor
    NeuralNetwork(const vector<int>& topology, double learningRate= double(0.005));
    NeuralNetwork(istream &in);

    // function for forward propagation of data
    void propagateForward(const RowVector& input);

    // function for backward propagation of errors made by neurons
    void propagateBackward(const RowVector& output);

    // function to calculate errors made by neurons in each layer
    void calcErrors(const RowVector& output);

    // function to update the weights of connections
    void updateWeights();

    // function to train the neural network give an array of data points
    //void train(vector<RowVector*>& data);
    void train(data& input_data, data& output_data, int epochs=1);
    double trainsingle(RowVector input, RowVector output, bool prin=false);

    int testsingle(RowVector input);

    void print(ostream &out=cout);
    void updateLR(double newLR){
        learningRate=newLR;
    }
    // storage objects for working of neural network
    /*
          use pointers when using std::vector<Class> as std::vector<Class> calls destructor of
          Class as soon as it is pushed back! when we use pointers it can't do that, besides
          it also makes our neural network class less heavy!! It would be nice if you can use
          smart pointers instead of usual ones like this
        */
    vector<RowVector> neuronLayers; // stores the different layers of out network
    //vector<RowVector> cacheLayers; // stores the unactivated (activation fn not yet applied) values of layers
    vector<RowVector> deltas; // stores the error contribution of each neurons
    vector<Matrix> weights; // the connection weights itself
    vector<int> topology;
    double learningRate;

    /*int stabs(const vector<double>& queryvector){ //infence function
        return 0;//query \cup pointset = \emptyset
        return 1;//query \cup pointset = pointset
        return 2;//query stabs pointset
    }*/

    ~NeuralNetwork(){}

    ColVector output2class(Matrix& M){
        int rows=M.rows();
        ColVector ans(rows);

        for(int i=0;i<rows;i++){
            if(M(i,0)==1)ans(i)=0;
            else if(M(i,1)==1)ans(i)=1;
            else ans(i)=2;
        }
        return ans;
    }
    RowVector class2output(int c){
        RowVector ans(3);
        if(c==0)ans<<1,0,0;
        else if(c==1)ans<<0,1,0;
        else ans<<0,0,1;
        return ans;
    }
private:

    static double activationFunction(double x)
    {
        if(x>0)return x;
        return 0;
    }

    static double activationFunctionDerivative(double x)
    {
        if(x>0)return 1;
        return 0;
    }
    static double activationFunctionDerTermsAcFun(double x)
    {
        if(x>0)return 1;
        return 0;
    }
};

// constructor of neural network class
NeuralNetwork::NeuralNetwork(const std::vector<int>& topology, double learningRate)
    : topology(topology), learningRate(learningRate) {

    for (size_t i = 0; i <= topology.size(); ++i) {
        // Initialize neuron layers with bias neuron for all layers except the last one
        if (i >= topology.size() - 1) {
            neuronLayers.emplace_back(RowVector(topology[topology.size() - 1]));
        } else {
            neuronLayers.emplace_back(RowVector(topology[i] + 1)); // Extra space for bias
            neuronLayers.back()(topology[i]) = 1.0; // Set bias neuron
        }
        // Initialize cache and delta vectors
        if(i==topology.size())break;
        deltas.emplace_back(RowVector(neuronLayers.back().size()));
        if (i > 0) {
            // Initialize weights matrix
            if (i < topology.size() - 1) {
                weights.emplace_back(Matrix(topology[i - 1] + 1, topology[i] + 1)); // Bias in both
                weights.back().setRandom();
                weights.back().col(topology[i]).setZero(); // Zero out bias column
                weights.back()(topology[i - 1], topology[i]) = 1.0; // Bias connection
            } else{
                weights.emplace_back(Matrix(topology[i - 1] + 1, topology[i])); // No bias in last layer
                weights.back().setRandom();
            }
        }
    }
}

NeuralNetwork::NeuralNetwork(istream &in){
    int n;
    string str;
    in>>str>>n;
    topology.resize(4);
    in>>str;
    for(int i=0;i<n;i++){
	in>>topology[i];
    }
    in>>str>>learningRate;
    in>>str;
    for(unsigned int i = 0; i <= topology.size(); i++){
        // Initialize neuron layers with bias neuron for all layers except the last one
        if (i >= topology.size() - 1) {
            neuronLayers.emplace_back(RowVector(topology[topology.size() - 1]));
        } else {
            neuronLayers.emplace_back(RowVector(topology[i] + 1)); // Extra space for bias
            neuronLayers.back()(topology[i]) = 1.0; // Set bias neuron
        }
        // Initialize cache and delta vectors
        if(i==topology.size())break;
        deltas.emplace_back(RowVector(neuronLayers.back().size()));
        if (i > 0 && i<topology.size()) {
            // Initialize weights matrix
            if (i < topology.size() - 1) {
                //weights.emplace_back(Matrix(topology[i - 1] + 1, topology[i] + 1)); // Bias in both
		int t1,t2;
                in>>t1>>t2;

		weights.emplace_back(Matrix(t1,t2));
                for(int j=0;j<t1;j++){
                    for(int y=0;y<t2;y++){
                        double tv;
                        in>>tv;
                        weights.back()(j,y)=tv;
                    }
                }
                //weights.back().setRandom();
                //weights.back().col(topology[i]).setZero(); // Zero out bias column
                //weights.back()(topology[i - 1], topology[i]) = 1.0; // Bias connection
            } else{
                unsigned int t1,t2;
                in>>t1>>t2;
		weights.emplace_back(Matrix(t1,t2));
                for(unsigned int j=0;j<t1;j++){
                    for(unsigned int y=0;y<t2;y++){
                        double tv;
                        in>>tv;
                        weights.back()(j,y)=tv;
                    }
                }
            }
        }
    }
}

void NeuralNetwork::propagateForward(const RowVector& input) {
    // Set the input to the first neuron layer (excluding bias)
    int tsize=topology.size();

    neuronLayers.front().head(neuronLayers.front().size() - 1) = input;
    // Propagate forward through the network
    for (int i = 1; i < tsize; ++i) {
        // Compute activations for the current layer
        neuronLayers[i] = (neuronLayers[i - 1] * weights[i - 1]).unaryExpr(&activationFunction);
    }
    double sum=0.0;
    for(int i=0;i<topology[tsize-1];i++){
        sum+=exp(neuronLayers[tsize-1](i));
    }
    for(int i=0;i<topology[tsize-1];i++){
        neuronLayers[tsize](i)= exp(neuronLayers[tsize-1](i))/sum;
    }
}
void NeuralNetwork::calcErrors(const RowVector& output) {
    // Calculate the errors for the output layer
    deltas.back() = neuronLayers.back() - output;

    // Calculate errors for hidden layers
    for (int i = topology.size() - 2; i > 0; --i) {
        // Compute error for current layer
        deltas[i] = (deltas[i + 1] * weights[i].transpose()).cwiseProduct(
            neuronLayers[i].unaryExpr([](double x) { return activationFunctionDerTermsAcFun(x); })
        );
    }
}

void NeuralNetwork::updateWeights() {
    // Iterate through each layer's weights (excluding input layer)
    for (size_t i = 0; i < weights.size(); ++i) {
        // Compute the gradient for the weight update
        Eigen::MatrixXd gradient = neuronLayers[i].transpose() * deltas[i + 1];

        // Update weights using gradient and learning rate
        weights[i] -= learningRate * gradient;
    }
}

void NeuralNetwork::propagateBackward(const RowVector& output) {
    // Calculate the errors based on the output
    calcErrors(output);

    // Update the weights using the calculated errors
    updateWeights();
}

double NeuralNetwork::trainsingle(RowVector input, RowVector output, bool prin){
    propagateForward(input);

    propagateBackward(output);

    double mse = sqrt(deltas.back().squaredNorm() / deltas.back().size());
    if(prin){
        cout << "Expected output: " << output << std::endl;
        cout << "Output produced: " << neuronLayers.back() << std::endl;
        cout<<"MSE: "<<mse<<endl;
    }
    return mse;
}

int NeuralNetwork::testsingle(RowVector input){
    propagateForward(input);

    static random_device rd;
    static mt19937 gen(rd());
    uniform_real_distribution<double> dis(0.0, 1.0);

    double random_value = dis(gen);

    double p1=neuronLayers.back()(0);
    double p2=neuronLayers.back()(1)+p1;

    if(random_value<p1)return 0;
    else if(random_value<p2)return 1;
    else return 2;
}


void NeuralNetwork::train(data& input_data, data& output_data, int epochs) {

    vector<int> perm;

    for(unsigned int i=0;i<input_data.size();i++){
        perm.push_back(i);
    }
    random_device rd;
    mt19937 g(rd());

    for(int epoch=0;epoch<epochs;epoch++){
        cout<<endl<<"epoch "<<epoch<<endl;
        double tmse=0;
        shuffle(perm.begin(), perm.end(), g);
        for (size_t i = 0; i < input_data.size(); ++i) {
            int cur=perm[i];
            // Forward propagation
            propagateForward(input_data[cur]);


            // Display expected and actual outputs
            if(i%100==0){
                cout << "Expected output: " << output_data[cur] << std::endl;
                cout << "Output produced: " << neuronLayers.back() << std::endl;
            }
            // Backward propagation
            propagateBackward(output_data[cur]);
            // Calculate and display mean squared error (MSE)
            double mse = sqrt(deltas.back().squaredNorm() / deltas.back().size());
            tmse+=mse;
        }
        tmse/=input_data.size();
        cout<<"Epoch mean MSE: "<<tmse<<endl;
    }
}

void NeuralNetwork::print(ostream &out){
    out<<"TopologySize: "<<topology.size()<<endl;
    out<<"Topology: ";
    for(unsigned int i=0;i<topology.size();i++){
        out<<topology[i];
        if(i!=topology.size()-1)out<<" ";
    }
    out<<endl;
    out<<"LearningRate: "<<learningRate<<endl;


    out<<"Weights:"<<endl;

    for(unsigned int i=0;i<weights.size();i++){
        out<<weights[i].rows()<<" "<<weights[i].cols()<<endl;
        out<<weights[i]<<endl;
    }
}
