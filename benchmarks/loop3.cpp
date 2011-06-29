
// loop3 generated by makeloops.py Wed Jun 29 17:04:04 2011

#include <blitz/vector2.h>
#include <blitz/array.h>
#include <random/uniform.h>
#include <blitz/benchext.h>

#ifdef BZ_HAVE_VALARRAY
 #define BENCHMARK_VALARRAY
#endif

#ifdef BENCHMARK_VALARRAY
#include <valarray>
#endif

BZ_NAMESPACE(blitz)
extern void sink();
BZ_NAMESPACE_END

BZ_USING_NAMESPACE(blitz)
BZ_USING_NAMESPACE(std)

#if defined(BZ_FORTRAN_SYMBOLS_WITH_TRAILING_UNDERSCORES)
 #define loop3_f77 loop3_f77_
 #define loop3_f77overhead loop3_f77overhead_
 #define loop3_f90 loop3_f90_
 #define loop3_f90overhead loop3_f90overhead_
#elif defined(BZ_FORTRAN_SYMBOLS_WITH_DOUBLE_TRAILING_UNDERSCORES)
 #define loop3_f77 loop3_f77__
 #define loop3_f77overhead loop3_f77overhead__
 #define loop3_f90 loop3_f90__
 #define loop3_f90overhead loop3_f90overhead__
#elif defined(BZ_FORTRAN_SYMBOLS_CAPS)
 #define loop3_f77 LOOP3_F77
 #define loop3_f77overhead LOOP3_F77OVERHEAD
 #define loop3_f90 LOOP3_F90
 #define loop3_f90overhead LOOP3_F90OVERHEAD
#endif

extern "C" {
  void loop3_f77(const int& N, double* x, double* y, const double& a);
  void loop3_f77overhead(const int& N, double* x, double* y, const double& a);
  void loop3_f90(const int& N, double* x, double* y, const double& a);
  void loop3_f90overhead(const int& N, double* x, double* y, const double& a);

}

void VectorVersion(BenchmarkExt<int>& bench, double a);
void ArrayVersion(BenchmarkExt<int>& bench, double a);
void ArrayVersion_unaligned(BenchmarkExt<int>& bench, double a);
void ArrayVersion_misaligned(BenchmarkExt<int>& bench, double a);
void ArrayVersion_index(BenchmarkExt<int>& bench, double a);
void doTinyVectorVersion(BenchmarkExt<int>& bench, double a);
void F77Version(BenchmarkExt<int>& bench, double a);
#ifdef FORTRAN_90
void F90Version(BenchmarkExt<int>& bench, double a);
#endif
#ifdef BENCHMARK_VALARRAY
void ValarrayVersion(BenchmarkExt<int>& bench, double a);
#endif

const int numSizes = 20;
const int Nmax=1<<(numSizes-1);
const int tvNmax=7;
const bool runvector=false; // no point as long as Vector is Array<1>

int main()
{
    int numBenchmarks = 6;
    if (runvector) numBenchmarks++;
#ifdef BENCHMARK_VALARRAY
    numBenchmarks++;
#endif
#ifdef FORTRAN_90
    numBenchmarks++;
#endif

    BenchmarkExt<int> bench("loop3: $y = $y + a*$x", numBenchmarks);

    bench.setNumParameters(numSizes);

    Array<int,1> parameters(numSizes);
    Array<long,1> iters(numSizes);
    Array<double,1> flops(numSizes);

    for (int i=0; i < numSizes; ++i)
    {
      parameters(i) = Nmax>>i;
      iters(i) = 50000000L / parameters(i);
	
        if (iters(i) < 2)
            iters(i) = 2;
        flops(i) = 2 * parameters(i);
    }

    bench.setParameterVector(parameters);
    bench.setIterations(iters);
    bench.setOpsPerIteration(flops);
    bench.setDependentVariable("flops");
    bench.beginBenchmarking();

    double a = 0.39123982498157938742;


    ArrayVersion(bench, a);
    ArrayVersion_unaligned(bench, a);
    ArrayVersion_misaligned(bench, a);
    ArrayVersion_index(bench, a);
    doTinyVectorVersion(bench, a);
    F77Version(bench, a);
#ifdef FORTRAN_90
    F90Version(bench, a);
#endif
#ifdef BENCHMARK_VALARRAY
    ValarrayVersion(bench, a);
#endif

    if(runvector)
      VectorVersion(bench, a);

    bench.endBenchmarking();

    bench.saveMatlabGraph("loop3.m");
    return 0;
}

template<class T>
void initializeRandomDouble(T* data, int numElements, int stride = 1)
{
    ranlib::Uniform<T> rnd;

    for (int i=0; i < numElements; ++i)
        data[size_t(i*stride)] = rnd.random();
}

template<class T>
void initializeRandomDouble(valarray<T>& data, int numElements, int stride = 1)
{
    ranlib::Uniform<T> rnd;

    for (int i=0; i < numElements; ++i)
        data[size_t(i*stride)] = rnd.random();
}

void VectorVersion(BenchmarkExt<int>& bench, double a)
{
    bench.beginImplementation("Vector<T>");

    while (!bench.doneImplementationBenchmark())
    {
        int N = bench.getParameter();
        long iters = bench.getIterations();

        cout << bench.currentImplementation() << ": N = " << N << endl;

        Vector<double> x(N);
        initializeRandomDouble(x.data(), N);
        Vector<double> y(N);
        initializeRandomDouble(y.data(), N);


        bench.start();
        for (long i=0; i < iters; ++i)
        {
            y = y + a*x;
            sink();
        }
        bench.stop();

        bench.startOverhead();
        for (long i=0; i < iters; ++i) {
            sink();
	}

        bench.stopOverhead();
    }

    bench.endImplementation();
}


  void ArrayVersion(BenchmarkExt<int>& bench, double a)
{
    bench.beginImplementation("Array<T,1>");

    while (!bench.doneImplementationBenchmark())
    {
        int N = bench.getParameter();
        long iters = bench.getIterations();

        cout << bench.currentImplementation() << ": N = " << N << endl;

        Array<double,1> x(N);
        initializeRandomDouble(x.dataFirst(), N);
        Array<double,1> y(N);
        initializeRandomDouble(y.dataFirst(), N);


        bench.start();
        for (long i=0; i < iters; ++i)
        {
            y = y + a*x;
            sink();
        }
        bench.stop();

        bench.startOverhead();
        for (long i=0; i < iters; ++i) {
            sink();
	}

        bench.stopOverhead();
    }

    bench.endImplementation();
}


  void ArrayVersion_index(BenchmarkExt<int>& bench, double a)
{
    bench.beginImplementation("Array<T,1> (indexexpr.)");

    while (!bench.doneImplementationBenchmark())
    {
        int N = bench.getParameter();
        long iters = bench.getIterations();

        cout << bench.currentImplementation() << ": N = " << N << endl;

        Array<double,1> x(N);
        initializeRandomDouble(x.dataFirst(), N);
        Array<double,1> y(N);
        initializeRandomDouble(y.dataFirst(), N);


        bench.start();
        for (long i=0; i < iters; ++i)
        {
            y = y(tensor::i) + a*x(tensor::i);;
            sink();
        }
        bench.stop();

        bench.startOverhead();
        for (long i=0; i < iters; ++i) {
            sink();
	}

        bench.stopOverhead();
    }

    bench.endImplementation();
}

  void ArrayVersion_unaligned(BenchmarkExt<int>& bench, double a)
{
    bench.beginImplementation("Array<T,1> (unal.)");

    while (!bench.doneImplementationBenchmark())
    {
        int N = bench.getParameter();
        long iters = bench.getIterations();

        cout << bench.currentImplementation() << ": N = " << N << endl;


    Array<double,1> xfill(N+1);
    Array<double,1> x(xfill(Range(1,N)));
    initializeRandomDouble(x.dataFirst(), N);

    Array<double,1> yfill(N+1);
    Array<double,1> y(yfill(Range(1,N)));
    initializeRandomDouble(y.dataFirst(), N);


        bench.start();
        for (long i=0; i < iters; ++i)
        {
            y = y + a*x;
            sink();
        }
        bench.stop();

        bench.startOverhead();
        for (long i=0; i < iters; ++i) {
            sink();
	}

        bench.stopOverhead();
    }

    bench.endImplementation();
}

  void ArrayVersion_misaligned(BenchmarkExt<int>& bench, double a)
{
    bench.beginImplementation("Array<T,1> (misal.)");

    while (!bench.doneImplementationBenchmark())
    {
        int N = bench.getParameter();
        long iters = bench.getIterations();

        cout << bench.currentImplementation() << ": N = " << N << endl;


    Array<double,1> xfill(N+2);
    Array<double,1> x(xfill(Range(0,N+0-1)));
    initializeRandomDouble(x.dataFirst(), N);

    Array<double,1> yfill(N+2);
    Array<double,1> y(yfill(Range(1,N+1-1)));
    initializeRandomDouble(y.dataFirst(), N);


        bench.start();
        for (long i=0; i < iters; ++i)
        {
            y = y + a*x;
            sink();
        }
        bench.stop();

        bench.startOverhead();
        for (long i=0; i < iters; ++i) {
            sink();
	}

        bench.stopOverhead();
    }

    bench.endImplementation();
}


template<int N>
void TinyVectorVersion(BenchmarkExt<int>& bench, double a)
{
        cout << bench.currentImplementation() << ": N = " << N << endl;

        const int sz = bench.getParameter();
        assert(N==sz);
                           
        long iters = bench.getIterations();

        TinyVector<double,N> x(N);
        initializeRandomDouble(x.dataFirst(), N);
        TinyVector<double,N> y(N);
        initializeRandomDouble(y.dataFirst(), N);


        bench.start();
        for (long i=0; i < iters; ++i)
        {
            y = y + a*x;
            sink();
        }
        bench.stop();

        bench.startOverhead();
        for (long i=0; i < iters; ++i) {
            sink();
	}
        bench.stopOverhead();

	TinyVectorVersion<N>>1>(bench, a);
}

// end recursion
template<>
void TinyVectorVersion<0>(BenchmarkExt<int>& bench, double a)
{}

void doTinyVectorVersion(BenchmarkExt<int>& bench, double a)
{
  bench.beginImplementation("TinyVector<T>");
  // can't run tinyvector with full length because meta-unrolling
  // kills compiler...
  int N=Nmax;
  while(N> 1<<tvNmax) {
   bench.getParameter();
   bench.getIterations();
   bench.skip();
   N>>=1;
  }

  TinyVectorVersion< 1<<tvNmax >(bench, a);
  bench.endImplementation();
}


#ifdef BENCHMARK_VALARRAY
void ValarrayVersion(BenchmarkExt<int>& bench, double a)
{
    bench.beginImplementation("valarray<T>");

    while (!bench.doneImplementationBenchmark())
    {
        int N = bench.getParameter();
        cout << bench.currentImplementation() << ": N = " << N << endl;

        long iters = bench.getIterations();

        valarray<double> x(N);
        initializeRandomDouble(x, N);
        valarray<double> y(N);
        initializeRandomDouble(y, N);


        bench.start();
        for (long i=0; i < iters; ++i)
        {
            y = y + a*x;
            sink();
        }
        bench.stop();

        bench.startOverhead();
        for (long i=0; i < iters; ++i) {
	  sink();
	}
        bench.stopOverhead();
    }

    bench.endImplementation();
}
#endif

void F77Version(BenchmarkExt<int>& bench, double a)
{
    bench.beginImplementation("Fortran 77");

    while (!bench.doneImplementationBenchmark())
    {
        int N = bench.getParameter();
        cout << bench.currentImplementation() << ": N = " << N << endl;

        int iters = bench.getIterations();

        double* x = new double[N];
        initializeRandomDouble(x, N);
        double* y = new double[N];
        initializeRandomDouble(y, N);
        

        bench.start();
        for (int iter=0; iter < iters; ++iter)
            loop3_f77(N, x, y, a);
        bench.stop();

        bench.startOverhead();
        for (int iter=0; iter < iters; ++iter)
            loop3_f77overhead(N, x, y, a);

        bench.stopOverhead();

        delete [] x;
        delete [] y;

    }

    bench.endImplementation();
}

#ifdef FORTRAN_90
void F90Version(BenchmarkExt<int>& bench, double a)
{
    bench.beginImplementation("Fortran 90");

    while (!bench.doneImplementationBenchmark())
    {
        int N = bench.getParameter();
        cout << bench.currentImplementation() << ": N = " << N << endl;

        int iters = bench.getIterations();

        double* x = new double[N];
        initializeRandomDouble(x, N);
        double* y = new double[N];
        initializeRandomDouble(y, N);


        bench.start();
        for (int iter=0; iter < iters; ++iter)
            loop3_f90(N, x, y, a);
        bench.stop();

        bench.startOverhead();
        for (int iter=0; iter < iters; ++iter)
            loop3_f90overhead(N, x, y, a);

        bench.stopOverhead();
        delete [] x;
        delete [] y;

    }

    bench.endImplementation();
}
#endif

