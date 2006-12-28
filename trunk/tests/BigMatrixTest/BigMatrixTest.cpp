/* Portions copyright (c) 2005-6 Stanford University and Michael Sherman.
 * Contributors:
 * 
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including 
 * without limitation the rights to use, copy, modify, merge, publish, 
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject
 * to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included 
 * in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "SimTKcommon.h"
#include "SimTKcommon/TemplatizedLapack.h"

#include <cstdlib> // for rand()
#include <cstdio>
#include <ctime>
#include <string>
#include <complex>
#include <iostream>
using std::cout;
using std::endl;
using std::complex;

using namespace SimTK;

// Numerical Recipes declarations.
namespace NR {
    template <class DP>
    void lubksb(const int N, const DP* a/*N,N*/, const int* indx/*N*/, 
                DP* b/*N*/);

    template <class DP>
    void ludcmp(const int N, DP* a/*N,N*/, int* indx/*N*/, DP &d);

    template <class DP>
    void luinvert(int N, DP* a/*N,N*/, DP* y/*N,N*/);
}

// Some explicit instantiations just to make sure everything's there.
template class Matrix_<Real>;
template class Vector_<Complex>;
template class RowVector_< conjugate<float> >;

template class MatrixView_< complex<long double> >;
template class VectorView_< negator<float> >;
template class RowVectorView_< negator< conjugate<float> > >;

template <class NT>
void dump(const String& s,  const Matrix_<NT>& mm) {
    cout << s << ":" << endl; 
    for (int i=0; i<mm.nrow(); ++i) {
        for (int j=0; j<mm.ncol(); ++j)
            cout << mm(i,j) << " ";
        cout << endl;
    }
}

template <class NT>
void dump(const String& s, const MatrixView_<NT>& mv)
    { dump(s,(const Matrix_<NT>&)mv); }

extern "C" {
    void SimTK_version_SimTKlapack(int* major, int* minor, int* build);
    void SimTK_about_SimTKlapack(const char* key, int maxlen, char* value);
}

int main()
{
  try {
    SimTK_DEBUG("Running BigMatrixTest ...\n");

    int major,minor,build;
    char out[100];
    const char* keylist[] = { "version", "library", "type", "debug", "authors", "copyright", "svn_revision", 0 };

    SimTK_version_SimTKlapack(&major,&minor,&build);
    std::printf("SimTKlapack library version: %d.%d.%d\n", major, minor, build);


    SimTK_version_SimTKcommon(&major,&minor,&build);
    std::printf("==> SimTKcommon library version: %d.%d.%d\n", major, minor, build);
    std::printf("    SimTK_about_SimTKcommon():\n");
    for (const char** p = keylist; *p; ++p) {
        SimTK_about_SimTKcommon(*p, 100, out);
        std::printf("      about(%s)='%s'\n", *p, out);
    }

    const Real vvvdata[] = {1,2,.1,.2,9,10,-22,-23,-24,25};
    Vector vvv(10,vvvdata);
    cout << "vvv=" << vvv << endl;
    cout << "vvv(2,5)=" << vvv(2,5) << endl;
    Vector vvv25;
    vvv25.viewAssign(vvv(2,5));
    cout << "vvv25=" << vvv25 << endl;
    cout << "vvv(2,0)=" << vvv(2,0) << endl;
    Vector vvv20;
    vvv20.viewAssign(vvv(2,0));
    cout << "vvv20=" << vvv20 << endl;
    cout << "vvv(0,0)=" << vvv(0,0) << endl;
    Vector vvv00;
    vvv00.viewAssign(vvv(0,0));
    cout << "vvv00=" << vvv00 << endl;
    Vector vb;
    vvv00 = vb;
    cout << "after vvv00=vb [null], vvv00(" << vvv00.nrow() << "," << vvv00.ncol() << ")=" << vvv00 << endl;

    const Complex mdc[] = { Complex(1.,2.),  
                            Complex(3.,4.),
                            Complex(5.,6.),
                            Complex(7.,8.),
                            Complex(9.,10.),
                            Complex(10.,11.),
                            Complex(.1,.26),  
                            Complex(.3,.45),
                            Complex(.5,.64),
                            Complex(.7,.83),
                            Complex(.9,.102),
                            Complex(.10,.111)   
                          };    
    Matrix_<Complex> md(2,2,mdc);
    cout << "2x2 complex Matrix md=" << md;
    Mat<2,2,Complex> md_mat(mdc);
    cout << "2x2 complex Mat md_mat=" << md_mat;
    cout << "  sizeof(Complex)=" << sizeof(Complex) << " sizeof(md_mat)=" << sizeof(md_mat) << endl;
    Matrix_<Mat<2,2,Complex> > mm22c;
    Mat<2,2, Mat<2,2,Complex> > mm22c_mat;
    cout << "  sizeof(mm22c_mat)=" << sizeof(mm22c_mat) << " should be " << 16*sizeof(Complex) << endl;
    mm22c.resize(2,2); cout << "  sizeof(mm22c(2,2))=" <<  ((char*)(&mm22c(1,1)(1,1)+1)) - ((char*)&mm22c(0,0)(0,0)) << endl;

    mm22c.setToZero(); cout << "mm22c after setToZero():" << mm22c;
    mm22c.setToNaN(); cout << "mm22c after setToNaN():" << mm22c;

    mm22c.dump("**** mm22c ****");

    cout << "~md=" << ~md;
    cout << "~md(1)=" << ~md(1) << endl;
    cout << "(~md)(1)=" << (~md)(1) << endl;
    cout << "~md[1]=" << ~md[1] << endl;
    cout << "(~md)[1]=" << (~md)[1] << endl;

    dump("2x2 complex md",md);
    dump("md(0,1,2,1)",md(0,1,2,1));
    const CMatrixView& mvc = md(0,1,2,1);
    dump("mvc=md(0,1,2,1)", mvc);
    md(1,0) *= 10.;
    dump("md after md(1,0) *= 10.",md);
    md(0,1,2,1) *= Complex(10.,100.);
    dump("md after md(0,1,2,1)*=(10+100i)",md);
        
    Matrix_<Complex> mm(3,4);
    mm = 2390.; 
    for (int i=0; i<mm.nrow(); ++i) {
        for (int j=0; j<mm.ncol(); ++j)
            mm(i,j)=i*j, cout << mm(i,j) << " ";
        cout << endl;
    }
    
    mm *= 1000.; dump("mm(3,4) after *=1000", mm);
    mm.dump("*** mm ***");

    cout << "mm using << op: " << mm;
    cout << "~mm: " << ~mm;
    cout << "mm.norm()=" << mm.norm() << endl;
    cout << "mm(3)=" << mm(3) << endl;
    cout << "mm[2]=" << mm[2] << endl;
    cout << "mm + 2*(-mm)=" << mm + 2*(-mm) << endl;

    Matrix_<Complex> mm2 = mm;
    cout << "mm2=" << mm2;
    cout << "mm2(1,1,1,2)=" << mm2(1,1,1,2);
    mm2(1,1,1,2)=7;
    cout << "after mm2(1112)=7, mm2=" << mm2;

    cout << "mm=" << mm;
    (~mm)(1,1,1,2)=99;
    cout << "after (~mm)(1112)=99, mm=" << mm;


    cout << "\n-------- ASSIGN TEST --------\n";
    Matrix_< Vec<2,Complex> > rr(2,3); 
    for (int i=0;i<2;++i) for (int j=0;j<3;++j) 
        rr(i,j) = Vec<2,Complex>(mdc[i]*(j+1), mdc[i]*(j-1));
    Matrix_< Vec<2,Complex> > rrAssign;
    (rrAssign = rr(0,1,2,2)) *= 1000.;
    cout << "rr=" << rr;
    cout << "(rrAssign=rr(0,1,2,2)) *= 1000.; rrAssign=" << rrAssign;
    cout << "rr=" << rr;
    (rrAssign.viewAssign(rr(0,1,2,2))) *= 100.;
    cout << "(rrAssign.viewAssign(rr(0,1,2,2))) *= 100; rrAssign=" << rrAssign;
    cout << "rr=" << rr;
    cout << "-------- END ASSIGN TEST --------\n\n";

    cout << "\n-------- RESIZE KEEP TEST --------\n";
    Vector resizeMe(5); for (int i=0; i<5; ++i) resizeMe[i]=i;
    cout << "resizeMe=" << resizeMe << endl;
    resizeMe.resize(10); 
    cout << "after resize(10), resizeMe=" << resizeMe << endl;
    Vector resizeMe2(5); for (int i=0; i<5; ++i) resizeMe2[i]=i;
    cout << "resizeMe2=" << resizeMe2 << endl;
    resizeMe2.resizeKeep(10); 
    cout << "after resizeKeep(10), resizeMe2=" << resizeMe2 << endl;
    Matrix resizeMem(2,3); for (int i=0; i<2; ++i) for (int j=0; j<3; ++j) resizeMem(i,j)=(i+1)*(j+1);
    cout << "resizeMem(2,3)=" << resizeMem << endl;
    resizeMem.resizeKeep(3,5); 
    cout << "after resizeKeep(3,5), resizeMem=" << resizeMem << endl;
    resizeMem.resizeKeep(2,2);
    cout << "after resizeKeep(2,2), resizeMem=" << resizeMem << endl;
    resizeMem.resize(3,4);
    cout << "after resize(3,4), resizeMem=" << resizeMem << endl;
    cout << "-------- END RESIZE KEEP TEST --------\n\n";

    Mat<3,4,Complex> cm34;
    for (int i=0; i<3; ++i)
        for (int j=0; j<4; ++j)
            cm34(i,j) = mdc[i+j*3];
    cout << "Mat<3,4,Complex>=" << cm34 << endl;
    Vec<4,Complex> cv4(&mdc[6]);
    cout << "Vec<4,Complex>=" << cv4 << endl;
    cout << "Mat<3,4>*Vec<4>=" << cm34*cv4 << endl;
    cout << "Mat<3,4>*Mat<4,3>=" << cm34*~cm34;

    complex<float> zzzz(1.,2.);
    conjugate<float> jjjj(0.3f,0.4f);
    negator<float> nnnn(7.1);
    cout << "zzzz=" << zzzz << " jjjj=" << jjjj << " nnnn=" << nnnn << endl;
    cout << "zzzz*jjjj=" << zzzz*jjjj << endl;

    Matrix_<Complex> cMatrix34(3,4);
    Vector_<Complex> cVector4(4);
    for (int i=0; i<3; ++i) for (int j=0; j<4; ++j) cMatrix34(i,j)=cm34(i,j);
    for (int i=0; i<4; ++i) cVector4[i] = cv4[i];
    cout << "cMatrix34=" << cMatrix34;
    cout << "cVector4=" << cVector4 << endl;
    cout << "cMatrix34*cVector4=" << cMatrix34*cVector4 << endl;
    cout << "cMatrix34*cMatrix43=" << cMatrix34*~cMatrix34;

    Vector vv(4), ww;
    vv[0] = 1.; vv[1] = 2.; vv[2] = 3.; vv[3] = 4.;
    cout << "vv(4)=" << vv << endl;
    vv *= 9.; cout << "vv*=9:" << vv << endl;

    Vector vvvv;
    vvvv = vv[2] * vv;
    cout << "vvvv = vv[2]*vv = " << vvvv << endl;

    Matrix mnm(4,2), nn;
    cout << "mnm(4,2)=" << mnm;
    mnm(0) = vv; 
    mnm(1) = -0.01 * vv;
    cout << "mnm(vv,-.01*vv)=" << mnm;
    cout << "===> mnm.abs()=" << mnm.abs();
    cout << "mnm(1)=" << mnm(1) << endl;
    cout << "mnm(1).abs()=" << mnm(1).abs() << endl;
    cout << "mnm[1]=" << mnm[1] << endl;
    cout << "mnm[1].abs()=" << mnm[1].abs() << endl;
    
    ww = vv;
    ww *= 0.1;
    vv += ww;
    cout << "ww=vv, *=0.1:" << ww << endl;
    cout << "vv+=ww:" << vv << endl; 

    const Real rdata[]={1,2,3,
                        9,.1,14,
                        2,6,9};
    Matrix_<negator<Real> > A(3,3, (negator<Real>*)rdata);

    Matrix AI = A.invert();
    cout << "A=" << A << "AI=" << AI << " A*AI=" << A*AI;

    A.dump("*** A ***");
    AI.dump("*** AI ***");

    Mat<3,3,negator<Real> > smallA((negator<Real>*)rdata);
    Mat<3,3,negator<Real> >::TInvert smallAI(smallA.invert());

    cout << "smallA=" << smallA << " inv(smallA)=" << smallAI;
    cout << "smallA*inv(smallA)=" << smallA*smallAI << "NORM="
         << (smallA*smallAI).norm() << endl;
    
    try {
    const double ddd[] = { 11, 12, 13, 14, 15, 16 }; 
    const float fddd[] = { 11, 12, 13, 14, 15, 16 }; 
    const complex<float> ccc[] = {  complex<float>(1.,2.),  
                                    complex<float>(3.,4.),
                                    complex<float>(5.,6.),
                                    complex<float>(7.,8.) };
    Vec< 2,complex<float> > cv2(ccc); 
    cout << "cv2=" << cv2 << endl;
    cout << "(cv2+cv2)/1000:" << (cv2 + cv2) / complex<float>(1000,0) 
              << endl; 
    
    cout << "cv2:  " << cv2 << endl;
    cout << "cv2T: " << cv2.transpose() << endl; 
    cout << "-cv2: " << -cv2 << endl;
    cout << "~cv2: " << ~cv2 << endl;
    cout << "-~cv2: " << -(~cv2) << endl;
    cout << "~-cv2: " << ~(-cv2) << endl; 
    cout << "~-cv2*10000: " << (~(-cv2))*10000.f << endl;  
        
    (~cv2)[1]=complex<float>(101.1f,202.3f);
    cout << "after ~cv2[1]=(101.1f,202.3f), cv2= " << cv2 << endl;    
    -(~cv2)[1]=complex<float>(11.1f,22.3f);
    cout << "after -~cv2[1]=(11.1f,22.3f), cv2= " << cv2 << endl; 
        
    Vec<3,float> dv2(fddd), ddv2(fddd+3);
    dv2[2] = 1000;
    cout << 100.* (ddv2 - dv2) / 1000. << endl; 
    
    cout << "dv2=" << dv2 << " dv2.norm()=" << dv2.norm() << endl;
    cout << "cv2=" << cv2 << " cv2.norm()=" << cv2.norm() << endl; 
       
    const Vec<3,float> v3c[] = {Vec<3,float>(fddd),Vec<3,float>(fddd+1)};

    Vector_< Vec<2, Vec<3,float> > > vflt(2);
    vflt[0] = Vec<2, Vec<3,float> >(v3c); 
    vflt[1] = 100*vflt[0]; 
    cout << "vflt 2xvec3=" << vflt << endl;


    cout << "vflt.getNScalarsPerElement()=" << vflt.getNScalarsPerElement() << endl;
    cout << "vflt.getPackedSizeofElement()=" << vflt.getPackedSizeofElement() << endl;
    cout << "sizeof(vflt)=" << sizeof(vflt) << " sizeof(vflt[0])=" << sizeof(vflt[0]) << endl;
    cout << "vflt.hasContiguousData()=" << vflt.hasContiguousData() << endl;
    cout << "vflt.getContiguousScalarDataLength()=" << vflt.getContiguousScalarDataLength() << endl;

    const float* p = vflt.getContiguousScalarData();
    cout << "vflt's raw data: " << endl;
    for (int i=0; i<vflt.getContiguousScalarDataLength(); ++i)
        cout << " " << p[i];
    cout << endl;

    float* newData = new float[12];
    float* oldData;
    for (int i=0; i<12; ++i) newData[i]=(float)-i;
    vflt.swapOwnedContiguousScalarData(newData, 12, oldData);

    cout << "after data swap, vflt=" << vflt << endl;
    cout << "old data =";
    for (int i=0; i<12; ++i) cout << " " << oldData[i];
    cout << endl;
    delete oldData;

    }
    catch(const Exception::Base& b)
    {
        cout << b.getMessage() << endl;
    }  

    typedef double P;
    const int N = 500;
    const int LUP = 1;
    Matrix_<P> big(N,N);
    for (int j=0; j<N; ++j)
        for (int i=0; i<N; ++i)
            big(i,j) = 1+(float)std::rand()/RAND_MAX;
    cout << "big.norm()=" << big.norm() << endl;


    cout << "INVERTING " << LUP << "x" << N << "x" << N 
        << (sizeof(P)==4 ? std::string(" float") : std::string(" double")) << endl << std::flush;

    Matrix_<P> flip(N,N), nrflip(N,N);

    std::clock_t start = std::clock();
    for (int i=0; i<LUP; ++i)
        flip = big.invert();
    cout << "... DONE -- " << (double)(std::clock()-start)/CLOCKS_PER_SEC << " seconds" << endl << std::flush;
    cout << "initial norm=" << big.norm() << " invert.norm()=" << flip.norm() << endl << std::flush;

    Matrix_<P> nrbig(N,N); nrbig=big;
    cout << "NOW INVERT WITH NR, initial norm=" << nrbig.norm() << endl << std::flush;
    start = std::clock();
    for (int i=0; i<LUP; ++i) {
        nrbig=big;
        NR::luinvert(N, &nrbig(0,0), &nrflip(0,0));
    }
    cout << "... DONE -- " << (double)(std::clock()-start)/CLOCKS_PER_SEC << " seconds" << endl << std::flush;
    cout << " nrinverse.norm()=" << nrflip.norm() << endl << std::flush;

    cout << "big.norm()=" << big.norm() << " flip.norm()=" << flip.norm() << endl << std::flush;
    Matrix_<P> ans(N,N);
    cout << "Multiplying ..." << endl << std::flush;
    start = std::clock();
    for (int i=0; i<LUP; ++i)
        Lapack::gemm('n','n',N,N,N,P(1),&big(0,0),N,&flip(0,0),N,P(0),&ans(0,0),N);
    cout << "... DONE -- " << (double)(std::clock()-start)/CLOCKS_PER_SEC << " seconds" << endl << std::flush;
    cout << "RMS (big*flip).norm() - 1=" << std::sqrt(ans.normSqr()/N)-1. << endl << std::flush;

    cout << "Multiplying with NR result ..." << endl << std::flush;
    start = std::clock();
    for (int i=0; i<LUP; ++i)
        Lapack::gemm('n','n',N,N,N,P(1),&big(0,0),N,&nrflip(0,0),N,P(0),&ans(0,0),N);
    cout << "... DONE -- " << (double)(std::clock()-start)/CLOCKS_PER_SEC << " seconds" << endl << std::flush;
    cout << "RMS (big*nrflip).norm() - 1=" << std::sqrt(ans.normSqr()/N)-1. << endl << std::flush;

    cout << "Multiplying by hand ..." << endl << std::flush;
    start = std::clock();
    const P* bigp = &big(0,0);
    const P* flipp = &flip(0,0);
    P*       ansp  = &ans(0,0);
    for (int l=0; l<LUP; ++l) {
        for (int j=0; j<N; ++j) {
            for (int i=0; i<N; ++i) {
                P sum = P(0); const int jN=j*N;
                for (int k=0; k<N; ++k)
                    sum += bigp[k*N+i] * flipp[jN+k];
                ansp[jN+i] = sum;
            }
        }
    }
    cout << "... DONE -- " << (double)(std::clock()-start)/CLOCKS_PER_SEC << " seconds" << endl << std::flush;
    cout << "RMS (big*flip).norm() - 1=" << std::sqrt(ans.normSqr()/N)-1. << endl << std::flush;
    return 0;
  }
  catch (const std::exception& e) {
      cout << e.what() << endl;
  }
}

// Numerical Recipes version 2.11 LU decomp and inversion via backsolve.
// This is the C++ version modified to use column-ordered consecutive
// storage.

namespace NR {

// Return 1d index for column ordered matrix with leading dim N.
#define X(i,j) j*N+i

template <class DP>
void luinvert(const int N, DP* a/*N,N*/, DP* y/*N,N*/) {
    assert(a && y);
    int* indx = new int[N];
    DP d;
    NR::ludcmp(N, a, indx, d);
    for (int j=0; j<N; ++j) {
        DP* col = &y[X(0,j)];
        for (int i=0; i<N; ++i) col[i]=DP(0);
        col[j] = DP(1);
        NR::lubksb(N,a,indx,col); // writes directly into y
    }

    delete indx;
}

template <class DP>
void lubksb(const int N, const DP* a/*N,N*/, const int* indx/*N*/, 
                DP* b/*N*/)
{
	int i,ii=0,ip,j;
	DP sum;

	for (i=0;i<N;i++) {
		ip=indx[i];
		sum=b[ip];
		b[ip]=b[i];
		if (ii != 0)
			for (j=ii-1;j<i;j++) sum -= a[X(i,j)]*b[j];
		else if (sum != 0.0)
			ii=i+1;
		b[i]=sum;
	}
	for (i=N-1;i>=0;i--) {
		sum=b[i];
		for (j=i+1;j<N;j++) sum -= a[X(i,j)]*b[j];
		b[i]=sum/a[X(i,i)];
	}
}

template <class DP>
void ludcmp(const int N, DP* a/*N,N*/, int* indx/*N*/, DP &d)
{
	const DP TINY=DP(1.0e-20);
	int i,imax,j,k;
	DP big,dum,sum,temp;

    DP* vv = new DP[N];
	d=DP(1);
	for (i=0;i<N;i++) {
		big=0.0;
		for (j=0;j<N;j++)
			if ((temp=fabs(a[X(i,j)])) > big) big=temp;
        if (big == 0.0) {
            std::cerr << "Singular matrix in routine ludcmp" << endl;
            assert(false);
            exit(1);
        }
		vv[i]=DP(1)/big;
	}
	for (j=0;j<N;j++) {
		for (i=0;i<j;i++) {
			sum=a[X(i,j)];
			for (k=0;k<i;k++) sum -= a[X(i,k)]*a[X(k,j)];
			a[X(i,j)]=sum;
		}
		big=0.0;
		for (i=j;i<N;i++) {
			sum=a[X(i,j)];
			for (k=0;k<j;k++) sum -= a[X(i,k)]*a[X(k,j)];
			a[X(i,j)]=sum;
			if ((dum=vv[i]*fabs(sum)) >= big) {
				big=dum;
				imax=i;
			}
		}
		if (j != imax) {
			for (k=0;k<N;k++) {
				dum=a[X(imax,k)];
				a[X(imax,k)]=a[X(j,k)];
				a[X(j,k)]=dum;
			}
			d = -d;
			vv[imax]=vv[j];
		}
		indx[j]=imax;
		if (a[X(j,j)] == 0.0) a[X(j,j)]=TINY;
		if (j != N-1) {
			dum=DP(1)/(a[X(j,j)]);
			for (i=j+1;i<N;i++) a[X(i,j)] *= dum;
		}
	}

    delete vv;
}


} // namespace NR

