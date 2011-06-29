
// loop25 generated by makeloops.py Wed Jun 29 17:36:24 2011

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
 #define loop25_f77 loop25_f77_
 #define loop25_f77overhead loop25_f77overhead_
 #define loop25_f90 loop25_f90_
 #define loop25_f90overhead loop25_f90overhead_
#elif defined(BZ_FORTRAN_SYMBOLS_WITH_DOUBLE_TRAILING_UNDERSCORES)
 #define loop25_f77 loop25_f77__
 #define loop25_f77overhead loop25_f77overhead__
 #define loop25_f90 loop25_f90__
 #define loop25_f90overhead loop25_f90overhead__
#elif defined(BZ_FORTRAN_SYMBOLS_CAPS)
 #define loop25_f77 LOOP25_F77
 #define loop25_f77overhead LOOP25_F77OVERHEAD
 #define loop25_f90 LOOP25_F90
 #define loop25_f90overhead LOOP25_F90OVERHEAD
#endif

extern "C" {
  void loop25_f77(const int& N, double* x, double* a, double* b, double* c, double* y, const double& u, const double& v, const double& w);
  void loop25_f77overhead(const int& N, double* x, double* a, double* b, double* c, double* y, const double& u, const double& v, const double& w);
  void loop25_f90(const int& N, double* x, double* a, double* b, double* c, double* y, const double& u, const double& v, const double& w);
  void loop25_f90overhead(const int& N, double* x, double* a, double* b, double* c, double* y, const double& u, const double& v, const double& w);

}

void VectorVersion(BenchmarkExt<int>& bench, double u, double v, double w);
void ArrayVersion(BenchmarkExt<int>& bench, double u, double v, double w);
void ArrayVersion_unaligned(BenchmarkExt<int>& bench, double u, double v, double w);
void ArrayVersion_misaligned(BenchmarkExt<int>& bench, double u, double v, double w);
void ArrayVersion_index(BenchmarkExt<int>& bench, double u, double v, double w);
void doTinyVectorVersion(BenchmarkExt<int>& bench, double u, double v, double w);
void F77Version(BenchmarkExt<int>& bench, double u, double v, double w);
#ifdef FORTRAN_90
void F90Version(BenchmarkExt<int>& bench, double u, double v, double w);
#endif
#ifdef BENCHMARK_VALARRAY
void ValarrayVersion(BenchmarkExt<int>& bench, double u, double v, double w);
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

    BenchmarkExt<int> bench("loop25: $x = u*$b; $y = v*$b + w*$a + u*$c", numBenchmarks);

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
        flops(i) = 6 * parameters(i);
    }

    bench.setParameterVector(parameters);
    bench.setIterations(iters);
    bench.setOpsPerIteration(flops);
    bench.setDependentVariable("flops");
    bench.beginBenchmarking();

    double u = 0.39123982498157938742;
    double v = 0.39123982498157938742;
    double w = 0.39123982498157938742;


    ArrayVersion(bench, u, v, w);
    ArrayVersion_unaligned(bench, u, v, w);
    ArrayVersion_misaligned(bench, u, v, w);
    ArrayVersion_index(bench, u, v, w);
    doTinyVectorVersion(bench, u, v, w);
    F77Version(bench, u, v, w);
#ifdef FORTRAN_90
    F90Version(bench, u, v, w);
#endif
#ifdef BENCHMARK_VALARRAY
    ValarrayVersion(bench, u, v, w);
#endif

    if(runvector)
      VectorVersion(bench, u, v, w);

    bench.endBenchmarking();

    bench.saveMatlabGraph("loop25.m");
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

void VectorVersion(BenchmarkExt<int>& bench, double u, double v, double w)
{
    bench.beginImplementation("Vector<T>");

    while (!bench.doneImplementationBenchmark())
    {
        int N = bench.getParameter();
        long iters = bench.getIterations();

        cout << bench.currentImplementation() << ": N = " << N << endl;

        Vector<double> x(N);
        initializeRandomDouble(x.data(), N);
        Vector<double> a(N);
        initializeRandomDouble(a.data(), N);
        Vector<double> b(N);
        initializeRandomDouble(b.data(), N);
        Vector<double> c(N);
        initializeRandomDouble(c.data(), N);
        Vector<double> y(N);
        initializeRandomDouble(y.data(), N);


        bench.start();
        for (long i=0; i < iters; ++i)
        {
            x = u*b; y = v*b + w*a + u*c;
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


  void ArrayVersion(BenchmarkExt<int>& bench, double u, double v, double w)
{
    bench.beginImplementation("Array<T,1>");

    while (!bench.doneImplementationBenchmark())
    {
        int N = bench.getParameter();
        long iters = bench.getIterations();

        cout << bench.currentImplementation() << ": N = " << N << endl;

        Array<double,1> x(N);
        initializeRandomDouble(x.dataFirst(), N);
        Array<double,1> a(N);
        initializeRandomDouble(a.dataFirst(), N);
        Array<double,1> b(N);
        initializeRandomDouble(b.dataFirst(), N);
        Array<double,1> c(N);
        initializeRandomDouble(c.dataFirst(), N);
        Array<double,1> y(N);
        initializeRandomDouble(y.dataFirst(), N);


        bench.start();
        for (long i=0; i < iters; ++i)
        {
            x = u*b; y = v*b + w*a + u*c;
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


  void ArrayVersion_index(BenchmarkExt<int>& bench, double u, double v, double w)
{
    bench.beginImplementation("Array<T,1> (indexexpr.)");

    while (!bench.doneImplementationBenchmark())
    {
        int N = bench.getParameter();
        long iters = bench.getIterations();

        cout << bench.currentImplementation() << ": N = " << N << endl;

        Array<double,1> x(N);
        initializeRandomDouble(x.dataFirst(), N);
        Array<double,1> a(N);
        initializeRandomDouble(a.dataFirst(), N);
        Array<double,1> b(N);
        initializeRandomDouble(b.dataFirst(), N);
        Array<double,1> c(N);
        initializeRandomDouble(c.dataFirst(), N);
        Array<double,1> y(N);
        initializeRandomDouble(y.dataFirst(), N);


        bench.start();
        for (long i=0; i < iters; ++i)
        {
            x = u*b(tensor::i); y = v*b(tensor::i) + w*a(tensor::i) + u*c(tensor::i);;
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

  void ArrayVersion_unaligned(BenchmarkExt<int>& bench, double u, double v, double w)
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

    Array<double,1> afill(N+1);
    Array<double,1> a(afill(Range(1,N)));
    initializeRandomDouble(a.dataFirst(), N);

    Array<double,1> bfill(N+1);
    Array<double,1> b(bfill(Range(1,N)));
    initializeRandomDouble(b.dataFirst(), N);

    Array<double,1> cfill(N+1);
    Array<double,1> c(cfill(Range(1,N)));
    initializeRandomDouble(c.dataFirst(), N);

    Array<double,1> yfill(N+1);
    Array<double,1> y(yfill(Range(1,N)));
    initializeRandomDouble(y.dataFirst(), N);


        bench.start();
        for (long i=0; i < iters; ++i)
        {
            x = u*b; y = v*b + w*a + u*c;
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

  void ArrayVersion_misaligned(BenchmarkExt<int>& bench, double u, double v, double w)
{
    bench.beginImplementation("Array<T,1> (misal.)");

    while (!bench.doneImplementationBenchmark())
    {
        int N = bench.getParameter();
        long iters = bench.getIterations();

        cout << bench.currentImplementation() << ": N = " << N << endl;


    Array<double,1> xfill(N+5);
    Array<double,1> x(xfill(Range(0,N+0-1)));
    initializeRandomDouble(x.dataFirst(), N);

    Array<double,1> afill(N+5);
    Array<double,1> a(afill(Range(1,N+1-1)));
    initializeRandomDouble(a.dataFirst(), N);

    Array<double,1> bfill(N+5);
    Array<double,1> b(bfill(Range(2,N+2-1)));
    initializeRandomDouble(b.dataFirst(), N);

    Array<double,1> cfill(N+5);
    Array<double,1> c(cfill(Range(3,N+3-1)));
    initializeRandomDouble(c.dataFirst(), N);

    Array<double,1> yfill(N+5);
    Array<double,1> y(yfill(Range(4,N+4-1)));
    initializeRandomDouble(y.dataFirst(), N);


        bench.start();
        for (long i=0; i < iters; ++i)
        {
            x = u*b; y = v*b + w*a + u*c;
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
void TinyVectorVersion(BenchmarkExt<int>& bench, double u, double v, double w)
{
        cout << bench.currentImplementation() << ": N = " << N << endl;

        const int sz = bench.getParameter();
        assert(N==sz);
                           
        long iters = bench.getIterations();

        TinyVector<double,N> x(N);
        initializeRandomDouble(x.dataFirst(), N);
        TinyVector<double,N> a(N);
        initializeRandomDouble(a.dataFirst(), N);
        TinyVector<double,N> b(N);
        initializeRandomDouble(b.dataFirst(), N);
        TinyVector<double,N> c(N);
        initializeRandomDouble(c.dataFirst(), N);
        TinyVector<double,N> y(N);
        initializeRandomDouble(y.dataFirst(), N);


        bench.start();
        for (long i=0; i < iters; ++i)
        {
            x = u*b; y = v*b + w*a + u*c;
            sink();
        }
        bench.stop();

        bench.startOverhead();
        for (long i=0; i < iters; ++i) {
            sink();
	}
        bench.stopOverhead();

	TinyVectorVersion<N>>1>(bench, u, v, w);
}

// end recursion
template<>
void TinyVectorVersion<0>(BenchmarkExt<int>& bench, double u, double v, double w)
{}

void doTinyVectorVersion(BenchmarkExt<int>& bench, double u, double v, double w)
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

  TinyVectorVersion< 1<<tvNmax >(bench, u, v, w);
  bench.endImplementation();
}


#ifdef BENCHMARK_VALARRAY
void ValarrayVersion(BenchmarkExt<int>& bench, double u, double v, double w)
{
    bench.beginImplementation("valarray<T>");

    while (!bench.doneImplementationBenchmark())
    {
        int N = bench.getParameter();
        cout << bench.currentImplementation() << ": N = " << N << endl;

        long iters = bench.getIterations();

        valarray<double> x(N);
        initializeRandomDouble(x, N);
        valarray<double> a(N);
        initializeRandomDouble(a, N);
        valarray<double> b(N);
        initializeRandomDouble(b, N);
        valarray<double> c(N);
        initializeRandomDouble(c, N);
        valarray<double> y(N);
        initializeRandomDouble(y, N);


        bench.start();
        for (long i=0; i < iters; ++i)
        {
            x = u*b; y = v*b + w*a + u*c;
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

void F77Version(BenchmarkExt<int>& bench, double u, double v, double w)
{
    bench.beginImplementation("Fortran 77");

    while (!bench.doneImplementationBenchmark())
    {
        int N = bench.getParameter();
        cout << bench.currentImplementation() << ": N = " << N << endl;

        int iters = bench.getIterations();

        double* x = new double[N];
        initializeRandomDouble(x, N);
        double* a = new double[N];
        initializeRandomDouble(a, N);
        double* b = new double[N];
        initializeRandomDouble(b, N);
        double* c = new double[N];
        initializeRandomDouble(c, N);
        double* y = new double[N];
        initializeRandomDouble(y, N);
        

        bench.start();
        for (int iter=0; iter < iters; ++iter)
            loop25_f77(N, x, a, b, c, y, u, v, w);
        bench.stop();

        bench.startOverhead();
        for (int iter=0; iter < iters; ++iter)
            loop25_f77overhead(N, x, a, b, c, y, u, v, w);

        bench.stopOverhead();

        delete [] x;
        delete [] a;
        delete [] b;
        delete [] c;
        delete [] y;

    }

    bench.endImplementation();
}

#ifdef FORTRAN_90
void F90Version(BenchmarkExt<int>& bench, double u, double v, double w)
{
    bench.beginImplementation("Fortran 90");

    while (!bench.doneImplementationBenchmark())
    {
        int N = bench.getParameter();
        cout << bench.currentImplementation() << ": N = " << N << endl;

        int iters = bench.getIterations();

        double* x = new double[N];
        initializeRandomDouble(x, N);
        double* a = new double[N];
        initializeRandomDouble(a, N);
        double* b = new double[N];
        initializeRandomDouble(b, N);
        double* c = new double[N];
        initializeRandomDouble(c, N);
        double* y = new double[N];
        initializeRandomDouble(y, N);


        bench.start();
        for (int iter=0; iter < iters; ++iter)
            loop25_f90(N, x, a, b, c, y, u, v, w);
        bench.stop();

        bench.startOverhead();
        for (int iter=0; iter < iters; ++iter)
            loop25_f90overhead(N, x, a, b, c, y, u, v, w);

        bench.stopOverhead();
        delete [] x;
        delete [] a;
        delete [] b;
        delete [] c;
        delete [] y;

    }

    bench.endImplementation();
}
#endif

