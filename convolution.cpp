// 1D convolution: vector of doubles
// 2D convolution: vector of vector of doubles

using Signal1D = std::vector<double>;
using Matrix2D = std::vector<std::vector<double>>;


Signal1D convolve1D(const Signal1D& signal, const Signal1D& kernel) {
    size_t signal_len = signal.size();
    size_t kernel_len = kernel.size();
    if (kernel_len > signal_len || kernel_len == 0) {
        return {};
    }

    // initialize output vector with length: VECTOR - KERNEL + 1
    size_t output_len = signal_len - kernel_len + 1;
    Signal1D output(output_len, 0.0);

    // for output element...
    for (size_t i = 0; i < output_len; ++i) {
        double sum = 0.0;

        // for every kernel element...
        for (size_t j = 0; j < kernel_len; ++j) {
            sum += signal[i + j] * kernel[j];       // multiply the right kernel and output element (output + kernel_local_index)
        }
        output[i] = sum;
    }

    return output;
}



Matrix2D convolve2D(const Matrix2D& input, const Matrix2D& kernel) {
    size_t in_rows = input.size();
    size_t in_cols = input[0].size();
    size_t k_rows = kernel.size();
    size_t k_cols = kernel[0].size();

    if (k_rows > in_rows || k_cols > in_cols || k_rows == 0 || k_cols == 0) {
        return {};
    }

    // CONV_OUT_ROWS = inRows - kernelRows + 1;
    // CONV_OUT_COLS = inCols - kernelCols + 1;
    size_t out_rows = in_rows - k_rows + 1;
    size_t out_cols = in_cols - k_cols + 1;
    Matrix2D output(out_rows, std::vector<double>(out_cols, 0.0));

    // for row and col:
    for (size_t r = 0; r < out_rows; ++r) {
        for (size_t c = 0; c < out_cols; ++c) {
            
            double sum = 0.0;
            // Multiply the kernel with the current overlapping window
            for (size_t kr = 0; kr < k_rows; ++kr) {
                for (size_t kc = 0; kc < k_cols; ++kc) {
                    sum += input[r + kr][c + kc] * kernel[kr][kc];          // [row + kernel_local_row][col + kernel_local_col]
                }
            }
            output[r][c] = sum;
        }
    }

    return output;
}


int main() {
    // --- 1D Example ---
    std::cout << "=== 1D CONVOLUTION ===\n";
    Signal1D signal = {1.0, 2.0, 3.0, 4.0, 5.0};
    Signal1D kernel1D = {1.0, 0.0, -1.0}; // Edge detector kernel

    std::cout << "Signal: "; print1D(signal);
    std::cout << "Kernel: "; print1D(kernel1D);
    
    Signal1D output1D = convolve1D(signal, kernel1D);
    std::cout << "Output: "; print1D(output1D);


    // --- 2D Example ---
    std::cout << "\n=== 2D CONVOLUTION ===\n";
    Matrix2D image = {
        {1.0, 1.0, 1.0, 0.0, 0.0},
        {0.0, 1.0, 1.0, 1.0, 0.0},
        {0.0, 0.0, 1.0, 1.0, 1.0},
        {0.0, 0.0, 1.0, 1.0, 0.0},
        {0.0, 1.0, 1.0, 0.0, 0.0}
    };

    Matrix2D kernel2D = {
        {1.0, 0.0},
        {0.0, -1.0}
    };

    std::cout << "Input Image:\n";   print2D(image);
    std::cout << "Kernel:\n";        print2D(kernel2D);

    Matrix2D output2D = convolve2D(image, kernel2D);
    std::cout << "Output Image:\n";  print2D(output2D);
}