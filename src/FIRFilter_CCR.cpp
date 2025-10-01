#include "FIRFilter_CCR.hpp"


FIRFilter::FIRFilter(const int n_elmts,const std::vector<double>& b)
        : Stateful()
        ,n_elmts(n_elmts)
        , buffer(2*b.size(), 0.0)
        , head(0)
		, size((int)b.size())
        , M(mipp::N<double>())
        , P((n_elmts-2*(b.size()-1))/mipp::N<double>())
		, b(b.size(), double(0))
         {
    assert(size > 0);
    assert(mipp::N<double>() > 1);

	if (P < 0)
		P = 0;

	for (size_t i = 0; i < b.size(); i++)
		this->b[i] = b[b.size() - 1 - i];


    const std::string name = "FIRFilter";
    this->set_name(name);

    auto& p = this->create_task("process");                                                  // Create the task
    size_t ps_input = this->template create_socket_in<double>(p, "input", this->n_elmts);    // Create the input socket
    size_t ps_output = this->template create_socket_out<double>(p, "output", this->n_elmts);   // Create the output socket

    // create the codelet
    this->create_codelet(
      p,
      [ps_input, ps_output](spu::module::Module& m, spu::runtime::Task& t, const size_t frame_id) -> int
      {
          // Recover the Module and Sockets in the codelet
          auto& filter = static_cast<FIRFilter&>(m);
          double* input = (double*)(t[ps_input].get_dataptr());
          double* output = (double*)(t[ps_output].get_dataptr());

          // Process the data
          filter.process(input, output);
          return spu::runtime::status_t::SUCCESS;
      });
}



void FIRFilter::process(const double* input, double* output) {

	auto cX_N1 = reinterpret_cast<const double* >(input);
	auto cY_N2 = reinterpret_cast<      double* >(output);

	int rest = this->n_elmts - this->P * this->M;

	for(auto i = 0; i < rest/2; i++)
		step(&cX_N1[i], &cY_N2[i]);

	mipp::Reg<double> ps = (double)0;
	mipp::Reg<double> ps0;
	mipp::Reg<double> ps1;
	mipp::Reg<double> ps2;
	mipp::Reg<double> ps3;

	mipp::Reg<double> reg_x0;
	mipp::Reg<double> reg_x1;
	mipp::Reg<double> reg_x2;
	mipp::Reg<double> reg_x3;

	mipp::Reg<double> reg_b0;
	mipp::Reg<double> reg_b1;
	mipp::Reg<double> reg_b2;
	mipp::Reg<double> reg_b3;

	size_t b_size = b.size();
	size_t b_size_unrolled4 = (b.size() / 4) * 4;

	for (auto i = rest; i < this->n_elmts ; i += this->M)
	{
		ps0 = (double)0;
		ps1 = (double)0;
		ps2 = (double)0;
		ps3 = (double)0;

		for (size_t k = 0; k < b_size_unrolled4; k += 4)
		{
			reg_b0 = b[k +0];
			reg_b1 = b[k +1];
			reg_b2 = b[k +2];
			reg_b3 = b[k +3];

			reg_x0 = &input[-2 * (b_size -1) + i + 2 * (k + 0)];
			reg_x1 = &input[-2 * (b_size -1) + i + 2 * (k + 1)];
			reg_x2 = &input[-2 * (b_size -1) + i + 2 * (k + 2)];
			reg_x3 = &input[-2 * (b_size -1) + i + 2 * (k + 3)];

			ps0 = mipp::fmadd(reg_b0, reg_x0, ps0); // same as 'ps0 += reg_b0 * reg_x0'
			ps1 = mipp::fmadd(reg_b1, reg_x1, ps1); // same as 'ps1 += reg_b1 * reg_x1'
			ps2 = mipp::fmadd(reg_b2, reg_x2, ps2); // same as 'ps2 += reg_b2 * reg_x2'
			ps3 = mipp::fmadd(reg_b3, reg_x3, ps3); // same as 'ps3 += reg_b3 * reg_x3'
		}

		ps0 += ps1;
		ps2 += ps3;
		ps = ps0 + ps2;

		for (size_t k = b_size_unrolled4; k < b.size(); k++)
		{
			reg_b0 = b[k];
			reg_x0.load(input - 2 * (b_size -1) + i + (2 * k));
			ps = mipp::fmadd(reg_b0, reg_x0, ps); // same as 'ps += reg_b0 * reg_x0'
		}

		ps.store(output + i);
	}

	int sz = this->n_elmts/2;
	std::copy(&cX_N1[sz - b.size()], &cX_N1[sz], &this->buffer[0]);
	std::copy(&cX_N1[sz - b.size()], &cX_N1[sz], &this->buffer[b.size()]);
	this->head = 0;
}



void FIRFilter::reset() {
    std::fill(buffer.begin(), buffer.end(), 0.0);
    head = 0;
}