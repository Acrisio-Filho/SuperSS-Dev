// Arquivo job.cpp
// Criado em 21/05/2017 por Acrisio
// Implementação da classe job

#include "job.h"
#include "../UTIL/exception.h"
#include "../UTIL/message_pool.h"
#include "../TYPE/stda_error.h"

using namespace stdA;

job::job(call_func _func, void* _arg1, void* _arg2) {
	job_func = _func;
	job_arg1 = _arg1;
	job_arg2 = _arg2;

	if (job_func == nullptr)
		throw exception("Parametro invalid. job::job()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::JOB, 1, 0));

#ifdef _DEBUG
	_smp::message_pool::getInstance().push(new message("Job criado com sucesso!"));
#endif
};

job::~job() {};

bool job::isValid() {
	return job_func != nullptr;
};

int job::execute_job() {

	if (job_func == nullptr)
		throw exception("Job Func is null job::execute_job()", STDA_MAKE_ERROR(STDA_ERROR_TYPE::JOB, 2, 0));

	return job_func(job_arg1, job_arg2);
};

job *job::newInstanceJob() {
	return new job(job_func, job_arg1, job_arg2);
};