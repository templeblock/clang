// RUN: %clang_cc1 -triple x86_64-apple-macos10.7.0 -verify -fopenmp -ferror-limit 100 %s

void foo() {
}

bool foobool(int argc) {
  return argc;
}

struct S1; // expected-note {{declared here}} expected-note{{forward declaration of 'S1'}}
extern S1 a;
class S2 {
  mutable int a;
public:
  S2():a(0) { }
  static float S2s; // expected-note {{predetermined as shared}}
};
const S2 b;
const S2 ba[5];
class S3 {
  int a;
public:
  S3():a(0) { }
};
const S3 c; // expected-note {{predetermined as shared}}
const S3 ca[5]; // expected-note {{predetermined as shared}}
extern const int f;  // expected-note {{predetermined as shared}}
class S4 { // expected-note {{'S4' declared here}}
  int a;
  S4();
public:
  S4(int v):a(v) { }
};
class S5 { // expected-note {{'S5' declared here}}
  int a;
  S5():a(0) {}
public:
  S5(int v):a(v) { }
};

S3 h;
#pragma omp threadprivate(h) // expected-note {{defined as threadprivate or thread local}}

int main(int argc, char **argv) {
  const int d = 5;  // expected-note {{predetermined as shared}}
  const int da[5] = { 0 }; // expected-note {{predetermined as shared}}
  S4 e(4); // expected-note {{'e' defined here}}
  S5 g(5); // expected-note {{'g' defined here}}
  int i;
  int &j = i; // expected-note {{'j' defined here}}
  #pragma omp task private // expected-error {{expected '(' after 'private'}} expected-error {{expected expression}}
  #pragma omp task private ( // expected-error {{expected expression}} expected-error {{expected ')'}} expected-note {{to match this '('}}
  #pragma omp task private () // expected-error {{expected expression}}
  #pragma omp task private (argc // expected-error {{expected ')'}} expected-note {{to match this '('}}
  #pragma omp task private (argc, // expected-error {{expected expression}} expected-error {{expected ')'}} expected-note {{to match this '('}}
  #pragma omp task private (argc > 0 ? argv[1] : argv[2]) // expected-error {{expected variable name}}
  #pragma omp task private (argc)
  #pragma omp task private (S1) // expected-error {{'S1' does not refer to a value}}
  #pragma omp task private (a, b, c, d, f) // expected-error {{private variable with incomplete type 'S1'}} expected-error 3 {{shared variable cannot be private}}
  #pragma omp task private (argv[1]) // expected-error {{expected variable name}}
  #pragma omp task private(ba)
  #pragma omp task private(ca) // expected-error {{shared variable cannot be private}}
  #pragma omp task private(da) // expected-error {{shared variable cannot be private}}
  #pragma omp task private(S2::S2s) // expected-error {{shared variable cannot be private}}
  #pragma omp task private(e, g) // expected-error 2 {{private variable must have an accessible, unambiguous default constructor}}
  #pragma omp task private(h) // expected-error {{threadprivate or thread local variable cannot be private}}
  #pragma omp task shared(i), private(i) // expected-error {{shared variable cannot be private}} expected-note {{defined as shared}}
  foo();
  #pragma omp parallel
  #pragma omp task shared(i)
  #pragma omp task private(i)
  #pragma omp task private(j) // expected-error {{arguments of OpenMP clause 'private' cannot be of reference type}}
  foo();
  #pragma omp for private(i)
  for (int k = 0; k < 10; ++k) {
    #pragma omp task private(i)
    foo();
  }
  #pragma omp parallel
  #pragma omp for firstprivate(i)
  for (int k = 0; k < 10; ++k) {
    #pragma omp task private(i)
    foo();
  }
  #pragma omp parallel
  #pragma omp for reduction(+:i)
  for (int k = 0; k < 10; ++k) {
    #pragma omp task private(i)
    foo();
  }
  #pragma omp parallel
  #pragma omp for lastprivate(i)
  for (int k = 0; k < 10; ++k) {
    #pragma omp task private(i)
    foo();
  }
  #pragma omp task private(i)
  for (int k = 0; k < 10; ++k) {
    #pragma omp task private(i)
    foo();
  }
  #pragma omp task firstprivate(i)
  for (int k = 0; k < 10; ++k) {
    #pragma omp task private(i)
    foo();
  }
  #pragma omp parallel reduction(+:i)
  for (int k = 0; k < 10; ++k) {
    #pragma omp task private(i)
    foo();
  }

  return 0;
}
