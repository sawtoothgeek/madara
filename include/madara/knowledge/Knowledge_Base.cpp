
#include "madara/knowledge/Knowledge_Base.h"
#include "madara/knowledge/Knowledge_Base_Impl.h"
#include "madara/logger/Logger.h"


#include <sstream>
#include <iostream>

#include "ace/OS_NS_Thread.h"
#include "ace/High_Res_Timer.h"
#include "ace/OS_NS_sys_socket.h"

madara::knowledge::Knowledge_Base::Knowledge_Base ()
: impl_ (new Knowledge_Base_Impl ()), context_ (0)
{
}

madara::knowledge::Knowledge_Base::Knowledge_Base (
  const std::string & host, int transport)
: impl_ (new Knowledge_Base_Impl (host, transport)), context_ (0)
{
}

madara::knowledge::Knowledge_Base::Knowledge_Base (
  const std::string & host, int transport, 
  const std::string & knowledge_domain)
: impl_ (new Knowledge_Base_Impl (host, transport, knowledge_domain)),
  context_ (0)
{
}

madara::knowledge::Knowledge_Base::Knowledge_Base (
  const std::string & host, const madara::transport::Settings & settings)
: impl_ (new Knowledge_Base_Impl (host, settings)), context_ (0)
{
}

madara::knowledge::Knowledge_Base::Knowledge_Base (
  const Knowledge_Base & original)
: impl_ (original.impl_), context_ (original.context_)
{
}

madara::knowledge::Knowledge_Base::~Knowledge_Base ()
{
  // taken care of by Refcounter class
}

/// Assignment operator.
void 
madara::knowledge::Knowledge_Base::operator= (
  const Knowledge_Base &original)
{
  // Refcounter class takes care of the internal decrements and
  // increments.
  if (this != &original)
  {
    impl_ = original.impl_;
    context_ = original.context_;
  }
}

void
madara::knowledge::Knowledge_Base::use (
  Thread_Safe_Context & original)
{
  impl_ = 0;
  context_ = &original;
}

void
madara::knowledge::Knowledge_Base::lock (void)
{
  if (impl_.get_ptr ())
  {
    impl_->lock ();
  }
  else if (context_)
  {
    context_->lock ();
  }
}

void
madara::knowledge::Knowledge_Base::unlock (void)
{
  if (impl_.get_ptr ())
  {
    impl_->unlock ();
  }
  else if (context_)
  {
    context_->unlock ();
  }
}

int
madara::knowledge::Knowledge_Base::get_log_level (void)
{
  int result (0);

  if (impl_.get_ptr ())
  {
    result = impl_->get_log_level ();
  }
  else if (context_)
  {
    result = context_->get_log_level ();
  }

  return result;
}

madara::logger::Logger &
madara::knowledge::Knowledge_Base::get_logger (void) const
{
  if (impl_.get_ptr ())
  {
    return impl_->get_logger ();
  }
  else
  {
    return context_->get_logger ();
  }
}

void
madara::knowledge::Knowledge_Base::attach_logger (
  logger::Logger & logger) const
{
  if (impl_.get_ptr ())
  {
    impl_->attach_logger (logger);
  }
  else
  {
    context_->attach_logger (logger);
  }
}

void
madara::knowledge::Knowledge_Base::set_log_level (int level)
{
  if (impl_.get_ptr ())
  {
    impl_->set_log_level (level);
  }
  else if (context_)
  {
    context_->set_log_level (level);
  }
}


void
madara::knowledge::Knowledge_Base::copy (
  const Knowledge_Base & source,
  const Copy_Set & copy_set,
  bool clean_copy)
{
  if (impl_.get_ptr () && source.impl_.get_ptr () != 0)
  {
    impl_->copy (*source.impl_.get_ptr (), copy_set, clean_copy);
  }
  else if (context_ && source.impl_.get_ptr () != 0)
  {
    Knowledge_Base_Impl * source_impl =
      (Knowledge_Base_Impl *)source.impl_.get_ptr ();
    Thread_Safe_Context * source_context = &(source_impl->get_context ());

    context_->copy (*source_context, copy_set, clean_copy);
  }
}

/// Applies current time and modified to all global variables and tries
/// to send them.
int
madara::knowledge::Knowledge_Base::apply_modified (
  const Eval_Settings & settings)
{
  int result = 0;
  
  if (impl_.get_ptr ())
  {
    result = impl_->apply_modified (settings);
  }
  else if (context_)
  {
    context_->apply_modified ();
  }

  return result;
}

void
madara::knowledge::Knowledge_Base::close_transport (void)
{
  if (impl_.get_ptr ())
  {
    impl_->close_transport ();
  }
}

madara::Knowledge_Record
madara::knowledge::Knowledge_Base::get (const std::string & key,
             const Knowledge_Reference_Settings & settings)
{
  Knowledge_Record result;

  if (impl_.get_ptr ())
  {
    result = impl_->get (key, settings);
  }
  else if (context_)
  {
    result = context_->get (key, settings);
  }

  return result;
}

madara::knowledge::Variable_Reference
madara::knowledge::Knowledge_Base::get_ref (const std::string & key,
             const Knowledge_Reference_Settings & settings)
{
  Variable_Reference var;
  
  if (impl_.get_ptr ())
  {
    var = impl_->get_ref (key, settings);
  }
  else if (context_)
  {
    var = context_->get_ref (key, settings);
  }

  return var;
}

madara::Knowledge_Record
madara::knowledge::Knowledge_Base::get (
  const Variable_Reference & variable,
  const Knowledge_Reference_Settings & settings)
{
  Knowledge_Record result;
  
  if (impl_.get_ptr ())
  {
    result = impl_->get (variable, settings);
  }
  else if (context_)
  {
    result = context_->get (variable, settings);
  }

  return result;
}

madara::Knowledge_Record
madara::knowledge::Knowledge_Base::retrieve_index (
  const std::string & key, size_t index,
  const Knowledge_Reference_Settings & settings)
{
  Knowledge_Record result;
  
  if (impl_.get_ptr ())
  {
    result = impl_->retrieve_index (key, index, settings);
  }
  else if (context_)
  {
    result = context_->retrieve_index (key, index, settings);
  }

  return result;
}

madara::Knowledge_Record
madara::knowledge::Knowledge_Base::retrieve_index (
  const Variable_Reference & variable, size_t index,
  const Knowledge_Reference_Settings & settings)
{
  Knowledge_Record result;
  
  if (impl_.get_ptr ())
  {
    result = impl_->retrieve_index (variable, index, settings);
  }
  else if (context_)
  {
    result = context_->retrieve_index (variable, index, settings);
  }

  return result;
}


int
madara::knowledge::Knowledge_Base::read_file (
  const std::string & knowledge_key, 
  const std::string & filename, 
  const Eval_Settings & settings)
{
  int result = 0;
  
  if (impl_.get_ptr ())
  {
    result = impl_->read_file (knowledge_key, filename, settings);
  }
  else if (context_)
  {
    result = context_->read_file (knowledge_key, filename, settings);
  }

  return result;
}

int
madara::knowledge::Knowledge_Base::read_file (
  const Variable_Reference & variable, 
  const std::string & filename, 
  const Eval_Settings & settings)
{
  int result = 0;
  
  if (impl_.get_ptr ())
  {
    result = impl_->read_file (variable, filename, settings);
  }
  else if (context_)
  {
    result = context_->read_file (variable, filename, settings);
  }

  return result;
}

int
madara::knowledge::Knowledge_Base::set_file (
  const std::string & key,
  const unsigned char * value, size_t size, 
  const Eval_Settings & settings)
{
  int result = -1;

  if (key != "")
  {
    if (impl_.get_ptr ())
    {
      result = impl_->set_file (
        impl_->get_ref (key, settings), value, size, settings);
    }
    else if (context_)
    {
      result = context_->set_file (
        context_->get_ref (key, settings), value, size, settings);
    }
  }

  return result;
}

int
madara::knowledge::Knowledge_Base::set_file (
  const Variable_Reference & variable,
  const unsigned char * value, size_t size, 
  const Eval_Settings & settings)
{
  int result = 0;
  
  if (impl_.get_ptr ())
  {
    result = impl_->set_file (variable, value, size, settings);
  }
  else if (context_)
  {
    result = context_->set_file (variable, value, size, settings);
  }

  return result;
}

int
madara::knowledge::Knowledge_Base::set_jpeg (
  const std::string & key,
  const unsigned char * value, size_t size, 
  const Eval_Settings & settings)
{
  int result = -1;

  if (key != "")
  {
    if (impl_.get_ptr ())
    {
      result = impl_->set_jpeg (
        impl_->get_ref (key, settings), value, size, settings);
    }
    else if (context_)
    {
      result = context_->set_jpeg (
        impl_->get_ref (key, settings), value, size, settings);
    }
  }

  return result;
}

int
madara::knowledge::Knowledge_Base::set_jpeg (
  const Variable_Reference & variable,
  const unsigned char * value, size_t size, 
  const Eval_Settings & settings)
{
  int result = 0;
  
  if (impl_.get_ptr ())
  {
    result = impl_->set_jpeg (variable, value, size, settings);
  }
  else if (context_)
  {
    result = context_->set_jpeg (variable, value, size, settings);
  }

  return result;
}

int
madara::knowledge::Knowledge_Base::read_policy (
  const std::string & knowledge_key, 
  const std::string & filename)
{
  int result = 0;
  
  if (impl_.get_ptr ())
  {
    result = impl_->read_policy (knowledge_key, filename);
  }

  return result;
}

ssize_t
madara::knowledge::Knowledge_Base::write_file (const std::string & knowledge_key, 
                     const std::string & filename)
{
  ssize_t result = 0;
  
  if (impl_.get_ptr ())
  {
    result = impl_->write_file (knowledge_key, filename);
  }

  return result;
}


std::string
madara::knowledge::Knowledge_Base::expand_statement (
  const std::string & statement)
{
  std::string result;
  
  if (impl_.get_ptr ())
  {
    result = impl_->expand_statement (statement);
  }
  else if (context_)
  {
    result = context_->expand_statement (statement);
  }

  return result;
}

int
madara::knowledge::Knowledge_Base::set (
  const std::string & key,
  madara::Knowledge_Record::Integer value,
  const Eval_Settings & settings)
{
  int result = 0;
  
  if (impl_.get_ptr ())
  {
    result = impl_->set (key, value, settings);
  }
  else if (context_)
  {
    result = context_->set (key, value, settings);
  }

  return result;
}

void
madara::knowledge::Knowledge_Base::mark_modified (
  const Variable_Reference & variable)
{
  if (impl_.get_ptr ())
  {
    impl_->mark_modified (variable);
  }
  else if (context_)
  {
    context_->mark_modified (variable);
  }
}

int
madara::knowledge::Knowledge_Base::set (
  const Variable_Reference & variable,
  madara::Knowledge_Record::Integer value,
  const Eval_Settings & settings)
{
  int result = 0;
  
  if (impl_.get_ptr ())
  {
    result = impl_->set (variable, value, settings);
  }
  else if (context_)
  {
    result = context_->set (variable, value, settings);
  }

  return result;
}

int
madara::knowledge::Knowledge_Base::set_index (
  const std::string & key,
  size_t index,
  madara::Knowledge_Record::Integer value,
  const Eval_Settings & settings)
{
  int result = 0;
  
  if (impl_.get_ptr ())
  {
    result = impl_->set_index (key, index, value, settings);
  }
  else if (context_)
  {
    result = context_->set_index (key, index, value, settings);
  }

  return result;
}

int
madara::knowledge::Knowledge_Base::set_index (
  const Variable_Reference & variable,
  size_t index,
  madara::Knowledge_Record::Integer value,
  const Eval_Settings & settings)
{
  int result = 0;
  
  if (impl_.get_ptr ())
  {
    result = impl_->set_index (variable, index, value, settings);
  }
  else if (context_)
  {
    result = context_->set_index (variable, index, value, settings);
  }

  return result;
}

int
madara::knowledge::Knowledge_Base::set (
  const std::string & key,
  const Knowledge_Record::Integer * value,
  uint32_t size,
  const Eval_Settings & settings)
{
  int result = 0;
  
  if (impl_.get_ptr ())
  {
    result = impl_->set (key, value, size, settings);
  }
  else if (context_)
  {
    result = context_->set (key, value, size, settings);
  }

  return result;
}

int
madara::knowledge::Knowledge_Base::set (
  const Variable_Reference & variable,
  const Knowledge_Record::Integer * value,
  uint32_t size,
  const Eval_Settings & settings)
{
  int result = 0;
  
  if (impl_.get_ptr ())
  {
    result = impl_->set (variable, value, size, settings);
  }
  else if (context_)
  {
    result = context_->set (variable, value, size, settings);
  }

  return result;
}

int
madara::knowledge::Knowledge_Base::set (
  const std::string & key,
  const std::vector <Knowledge_Record::Integer> & value,
  const Eval_Settings & settings)
{
  int result = 0;
  
  if (impl_.get_ptr ())
  {
    result = impl_->set (key, value, settings);
  }
  else if (context_)
  {
    result = context_->set (key, value, settings);
  }

  return result;
}

int
madara::knowledge::Knowledge_Base::set (
  const Variable_Reference & variable,
  const std::vector <Knowledge_Record::Integer> & value,
  const Eval_Settings & settings)
{
  int result = 0;
  
  if (impl_.get_ptr ())
  {
    result = impl_->set (variable, value, settings);
  }
  else if (context_)
  {
    result = context_->set (variable, value, settings);
  }

  return result;
}

int
madara::knowledge::Knowledge_Base::set (
  const std::string & key,
  double value,
  const Eval_Settings & settings)
{
  int result = 0;
  
  if (impl_.get_ptr ())
  {
    result = impl_->set (key, value, settings);
  }
  else if (context_)
  {
    result = context_->set (key, value, settings);
  }

  return result;
}

int
madara::knowledge::Knowledge_Base::set (
  const Variable_Reference & variable,
  double value,
  const Eval_Settings & settings)
{
  int result = 0;
  
  if (impl_.get_ptr ())
  {
    result = impl_->set (variable, value, settings);
  }
  else if (context_)
  {
    result = context_->set (variable, value, settings);
  }

  return result;
}

int
madara::knowledge::Knowledge_Base::set_index (
  const std::string & key,
  size_t index,
  double value,
  const Eval_Settings & settings)
{
  int result = 0;
  
  if (impl_.get_ptr ())
  {
    result = impl_->set_index (key, index, value, settings);
  }
  else if (context_)
  {
    result = context_->set_index (key, index, value, settings);
  }

  return result;
}

int
madara::knowledge::Knowledge_Base::set_index (
  const Variable_Reference & variable,
  size_t index,
  double value,
  const Eval_Settings & settings)
{
  int result = 0;
  
  if (impl_.get_ptr ())
  {
    result = impl_->set_index (variable, index, value, settings);
  }
  else if (context_)
  {
    result = context_->set_index (variable, index, value, settings);
  }

  return result;
}

int
madara::knowledge::Knowledge_Base::set (
  const std::string & key,
  const double * value,
  uint32_t size,
  const Eval_Settings & settings)
{
  int result = 0;
  
  if (impl_.get_ptr ())
  {
    result = impl_->set (key, value, size, settings);
  }
  else if (context_)
  {
    result = context_->set (key, value, size, settings);
  }

  return result;
}

int
madara::knowledge::Knowledge_Base::set (
  const Variable_Reference & variable,
  const double * value,
  uint32_t size,
  const Eval_Settings & settings)
{
  int result = 0;
  
  if (impl_.get_ptr ())
  {
    result = impl_->set (variable, value, size, settings);
  }
  else if (context_)
  {
    result = context_->set (variable, value, size, settings);
  }

  return result;
}

int
madara::knowledge::Knowledge_Base::set (
  const std::string & key,
  const std::vector <double> & value,
  const Eval_Settings & settings)
{
  int result = 0;
  
  if (impl_.get_ptr ())
  {
    result = impl_->set (key, value, settings);
  }
  else if (context_)
  {
    result = context_->set (key, value, settings);
  }

  return result;
}

int
madara::knowledge::Knowledge_Base::set (
  const Variable_Reference & variable,
  const std::vector <double> & value,
  const Eval_Settings & settings)
{
  int result = 0;
  
  if (impl_.get_ptr ())
  {
    result = impl_->set (variable, value, settings);
  }
  else if (context_)
  {
    result = context_->set (variable, value, settings);
  }

  return result;
}

int
madara::knowledge::Knowledge_Base::set (const std::string & key,
  const std::string & value,
  const Eval_Settings & settings)
{
  int result = 0;
  
  if (impl_.get_ptr ())
  {
    result = impl_->set (key, value, settings);
  }
  else if (context_)
  {
    result = context_->set (key, value, settings);
  }

  return result;
}

int
madara::knowledge::Knowledge_Base::set (
  const Variable_Reference & variable,
  const std::string & value,
  const Eval_Settings & settings)
{
  int result = 0;
  
  if (impl_.get_ptr ())
  {
    result = impl_->set (variable, value, settings);
  }
  else if (context_)
  {
    result = context_->set (variable, value, settings);
  }

  return result;
}


/// Set quality of writing to a variable
void 
madara::knowledge::Knowledge_Base::set_quality (
  const std::string & key, uint32_t quality,
  const Knowledge_Reference_Settings & settings)
{
  if (impl_.get_ptr ())
  {
    impl_->set_quality (key, quality, settings);
  }
  else if (context_)
  {
    context_->set_quality (key, quality, true, settings);
  }
}


bool
madara::knowledge::Knowledge_Base::exists (
  const std::string & key,
  const Knowledge_Reference_Settings & settings) const
{
  bool result = false;
  
  if (impl_.get_ptr ())
  {
    result = impl_->exists (key, settings);
  }
  else if (context_)
  {
    result = context_->exists (key, settings);
  }

  return result;
}

bool
madara::knowledge::Knowledge_Base::exists (
  const Variable_Reference & variable,
  const Knowledge_Reference_Settings & settings) const
{
  bool result = false;
  
  if (impl_.get_ptr ())
  {
    result = impl_->exists (variable, settings);
  }
  else if (context_)
  {
    result = context_->exists (variable, settings);
  }

  return result;
}

#ifdef _USE_CID_

void
madara::knowledge::Knowledge_Base::print_all_redeployment_results (
  std::ostream & output)
{
  return impl_->print_all_redeployment_results (output);
}

void
madara::knowledge::Knowledge_Base::run_all (void)
{
  return impl_->run_all ();
}

long
madara::knowledge::Knowledge_Base::start_latency (void)
{
  return impl_->start_latency ();
}

void
madara::knowledge::Knowledge_Base::print_my_latencies (
  std::ostream & output)
{
  impl_->print_my_latencies (output);
}

void
madara::knowledge::Knowledge_Base::print_all_latencies (
  std::ostream & output)
{
  impl_->print_all_latencies (output);
}

void
madara::knowledge::Knowledge_Base::print_all_summations (
  std::ostream & output)
{
  impl_->print_all_summations (output);
}

long
madara::knowledge::Knowledge_Base::vote (void)
{
  return impl_->vote ();
}

#endif // _USE_CID_

madara::transport::Settings &
madara::knowledge::Knowledge_Base::transport_settings (void)
{
  return impl_->transport_settings ();
}

void
madara::knowledge::Knowledge_Base::print (
  unsigned int level) const
{
  if (impl_.get_ptr ())
  {
    impl_->print (level);
  }
  else if (context_)
  {
    context_->print (level);
  }
}

void
madara::knowledge::Knowledge_Base::print_knowledge (
  unsigned int level) const
{
  if (impl_.get_ptr ())
  {
    impl_->print (level);
  }
  else if (context_)
  {
    context_->print (level);
  }
}

void
madara::knowledge::Knowledge_Base::to_string (
  std::string & target,
  const std::string & array_delimiter,
  const std::string & record_delimiter,
  const std::string & key_val_delimiter) const
{
  if (impl_.get_ptr ())
  {
    impl_->to_string (target,
      array_delimiter, record_delimiter, key_val_delimiter);
  }
  else if (context_)
  {
    context_->to_string (target,
      array_delimiter, record_delimiter, key_val_delimiter);
  }
}


void
madara::knowledge::Knowledge_Base::print (
  const std::string & statement, unsigned int level) const
{
  if (impl_.get_ptr ())
  {
    impl_->print (statement, level);
  }
  else if (context_)
  {
    context_->print (level);
  }
}

void
madara::knowledge::Knowledge_Base::clear (bool erase)
{
  if (impl_.get_ptr ())
  {
    impl_->clear (erase);
  }
  else if (context_)
  {
    context_->clear (erase);
  }
}

void
madara::knowledge::Knowledge_Base::clear_map (void)
{
  if (impl_.get_ptr ())
  {
    impl_->clear_map ();
  }
  else if (context_)
  {
    impl_->clear_map ();
  }
}

void
madara::knowledge::Knowledge_Base::acquire (void)
{
  if (impl_.get_ptr ())
  {
    impl_->acquire ();
  }
  else if (context_)
  {
    context_->lock ();
  }
}

void
madara::knowledge::Knowledge_Base::release (void)
{
  if (impl_.get_ptr ())
  {
    impl_->release ();
  }
  else if (context_)
  {
    context_->unlock ();
  }
}

#ifndef _MADARA_NO_KARL_

madara::knowledge::Compiled_Expression
madara::knowledge::Knowledge_Base::compile (
  const std::string & expression)
{
  Compiled_Expression result;
  
  if (impl_.get_ptr ())
  {
    result = impl_->compile (expression);
  }

  return result;
}

// evaluate a knowledge expression and choose to send any modifications
madara::Knowledge_Record
madara::knowledge::Knowledge_Base::evaluate (
  const std::string & expression,
  const Eval_Settings & settings)
{
  Knowledge_Record result;
  
  if (impl_.get_ptr ())
  {
    result = impl_->evaluate (expression, settings);
  }
  else if (context_)
  {
    Compiled_Expression ce = context_->compile (expression);
    result = context_->evaluate (ce, settings);
  }

  return result;
}

// evaluate a knowledge expression and choose to send any modifications
madara::Knowledge_Record
madara::knowledge::Knowledge_Base::evaluate (
  Compiled_Expression & expression,
  const Eval_Settings & settings)
{
  Knowledge_Record result;
  
  if (impl_.get_ptr ())
  {
    result = impl_->evaluate (expression, settings);
  }
  else if (context_)
  {
    result = context_->evaluate (expression, settings);
  }

  return result;
}

// evaluate a knowledge expression and choose to send any modifications
madara::Knowledge_Record
madara::knowledge::Knowledge_Base::evaluate (
  expression::Component_Node * root,
  const Eval_Settings & settings)
{
  Knowledge_Record result;
  
  if (impl_.get_ptr ())
  {
    result = impl_->evaluate (root, settings);
  }
  else if (context_)
  {
    result = context_->evaluate (root, settings);
  }

  return result;
}


// Defines a function
void
madara::knowledge::Knowledge_Base::define_function (
  const std::string & name,
    Knowledge_Record (*func) (const char *, Function_Arguments &, Variables &))
{
  if (impl_.get_ptr ())
  {
    impl_->define_function (name, func);
  }
  else if (context_)
  {
    context_->define_function (name, func);
  }
}

// Defines a function
void madara::knowledge::Knowledge_Base::define_function (
  const std::string & name,
    Knowledge_Record (*func) (Function_Arguments &, Variables &))
{
  if (impl_.get_ptr ())
  {
    impl_->define_function (name, func);
  }
  else if (context_)
  {
    context_->define_function (name, func);
  }
}

#ifdef _MADARA_JAVA_
// Defines a function
void madara::knowledge::Knowledge_Base::define_function (
  const std::string & name, jobject func)
{
  if (impl_.get_ptr ())
  {
    impl_->define_function (name, func);
  }
  else if (context_)
  {
    context_->define_function (name, func);
  }
}
#endif

#ifdef _MADARA_PYTHON_CALLBACKS_

// Defines a function
void madara::knowledge::Knowledge_Base::define_function (
  const std::string & name, boost::python::object callable)
{
  if (impl_.get_ptr ())
  {
    impl_->define_function (name, callable);
  }
  else if (context_)
  {
    context_->define_function (name, callable);
  }
}

#endif

/**
  * Defines a MADARA KaRL function
  * @param  name       name of the function
  * @param  expression KaRL function body       
  **/
void
madara::knowledge::Knowledge_Base::define_function (const std::string & name,
  const std::string & expression)
{
  if (impl_.get_ptr ())
  {
    impl_->define_function (name, expression);
  }
  else if (context_)
  {
    context_->define_function (name, expression);
  }
}
      
/**
  * Defines a MADARA KaRL function
  * @param  name       name of the function
  * @param  expression KaRL function body       
  **/
void
madara::knowledge::Knowledge_Base::define_function (const std::string & name,
  const Compiled_Expression & expression)
{
  if (impl_.get_ptr ())
  {
    impl_->define_function (name, expression);
  }
  else if (context_)
  {
    context_->define_function (name, expression);
  }
}

madara::Knowledge_Record
madara::knowledge::Knowledge_Base::wait (
  Compiled_Expression & expression, 
  const Wait_Settings & settings)
{
  Knowledge_Record result;

  if (context_)
  {
    /**
     * The only situation this can be useful will be if the thread safe
     * context is being used as a shared memory structure between
     * threads. This should not be used for processes communicating
     * together because the wait statement is unable to send modifieds
     * as it has zero concept of transports. The type of knowledge
     * base handled here is a facade for another knowledge base's
     * context.
     **/
    
    ACE_Time_Value current = ACE_High_Res_Timer::gettimeofday ();  
    ACE_Time_Value max_wait, sleep_time, next_epoch;
    ACE_Time_Value poll_frequency, last = current;

    if (settings.poll_frequency >= 0)
    {
      max_wait.set (settings.max_wait_time);
      max_wait = current + max_wait;
    
      poll_frequency.set (settings.poll_frequency);
      next_epoch = current + poll_frequency;
    }

    // print the post statement at highest log level (cannot be masked)
    if (settings.pre_print_statement != "")
      context_->print (settings.pre_print_statement, logger::LOG_ALWAYS);

    // lock the context
    context_->lock ();

    madara_logger_log (context_->get_logger (), logger::LOG_MAJOR,
      "Knowledge_Base::wait:" \
      " waiting on %s\n", expression.logic.c_str ());

    madara::Knowledge_Record last_value = expression.expression.evaluate (settings);

    madara_logger_log (context_->get_logger (), logger::LOG_DETAILED,
      "Knowledge_Base::wait:" \
      " completed first eval to get %s\n",
      last_value.to_string ().c_str ());
  
    send_modifieds ("Knowledge_Base:wait", settings);

    context_->unlock ();
  
    current = ACE_High_Res_Timer::gettimeofday ();

    // wait for expression to be true
    while (!last_value.to_integer () &&
      (settings.max_wait_time < 0 || current < max_wait))
    {
      madara_logger_log (context_->get_logger (), logger::LOG_DETAILED,
        "Knowledge_Base::wait:" \
        " current is %llu.%llu and max is %llu.%llu (poll freq is %f)\n",
        (unsigned long long)current.sec (),
        (unsigned long long)current.usec (),
        (unsigned long long)max_wait.sec (),
        (unsigned long long)max_wait.usec (),
        settings.poll_frequency);

      madara_logger_log (context_->get_logger (), logger::LOG_DETAILED,
        "Knowledge_Base::wait:" \
        " last value didn't result in success\n");

      // Unlike the other wait statements, we allow for a time based wait.
      // To do this, we allow a user to specify a 
      if (settings.poll_frequency > 0)
      {
        if (current < next_epoch)
        {
          sleep_time = next_epoch - current;
          madara::utility::sleep (sleep_time);
        }

        next_epoch = next_epoch + poll_frequency;
      }
      else
        context_->wait_for_change (true);

      // relock - basically we need to evaluate the tree again, and
      // we can't have a bunch of people changing the variables as 
      // while we're evaluating the tree.
      context_->lock ();


      madara_logger_log (context_->get_logger (), logger::LOG_MAJOR,
        "Knowledge_Base::wait:" \
        " waiting on %s\n", expression.logic.c_str ());

      last_value = expression.expression.evaluate (settings);

      madara_logger_log (context_->get_logger (), logger::LOG_DETAILED,
        "Knowledge_Base::wait:" \
        " completed eval to get %s\n",
        last_value.to_string ().c_str ());
  
      send_modifieds ("Knowledge_Base:wait", settings);

      context_->unlock ();
      context_->signal ();

      // get current time
      current = ACE_High_Res_Timer::gettimeofday ();

    } // end while (!last)
  
    if (current >= max_wait)
    {
      madara_logger_log (context_->get_logger (), logger::LOG_MAJOR,
        "Knowledge_Base::wait:" \
        " Evaluate did not succeed. Timeout occurred\n");
    }

    // print the post statement at highest log level (cannot be masked)
    if (settings.post_print_statement != "")
      context_->print (settings.post_print_statement, logger::LOG_ALWAYS);

    return last_value;
  }
  else if (impl_.get_ptr ())
  {
    result = impl_->wait (expression, settings);
  }

  return result;
}

madara::Knowledge_Record
madara::knowledge::Knowledge_Base::wait (
  const std::string & expression, 
  const Wait_Settings & settings)
{
  Knowledge_Record result;

  if (context_)
  {
    Compiled_Expression ce = context_->compile (expression);
    result = this->wait (ce, settings);
  }
  else if (impl_.get_ptr ())
  {
    result = impl_->wait (expression, settings);
  }

  return result;
}

#endif // _MADARA_NO_KARL_

void
madara::knowledge::Knowledge_Base::activate_transport (void)
{
  if (impl_.get_ptr ())
  {
    impl_->activate_transport ();
  }
}

size_t
madara::knowledge::Knowledge_Base::attach_transport (
  madara::transport::Base * transport)
{
  size_t result = 0;
  
  if (impl_.get_ptr ())
  {
    result = impl_->attach_transport (transport);
  }

  return result;
}

size_t
madara::knowledge::Knowledge_Base::get_num_transports (void)
{
  size_t result (0);
  
  if (impl_.get_ptr ())
  {
    result = impl_->get_num_transports ();
  }

  return result;
}

size_t
madara::knowledge::Knowledge_Base::attach_transport (const std::string & id,
        transport::Settings & settings)
{
  size_t result = 0;
  
  if (impl_.get_ptr ())
  {
    result = impl_->attach_transport (id, settings);
  }

  return result;
}

size_t
  madara::knowledge::Knowledge_Base::remove_transport (
  size_t index)
{
  size_t result = 0;
  
  if (impl_.get_ptr ())
  {
    result = impl_->remove_transport (index);
  }

  return result;
}

madara::knowledge::Thread_Safe_Context &
madara::knowledge::Knowledge_Base::get_context (void)
{
  Thread_Safe_Context * result = 0;
  
  if (context_)
  {
    result = context_;
  }
  else if (impl_.get_ptr ())
  {
    result = &(impl_->get_context ());
  }

  return *result;
}

void
madara::knowledge::Knowledge_Base::clear_modifieds (void)
{
  if (context_)
  {
    context_->reset_modified ();
  }
  else if (impl_.get_ptr ())
  {
    impl_->clear_modifieds ();
  }
}

int
madara::knowledge::Knowledge_Base::send_modifieds (
  const std::string & prefix,
  const Eval_Settings & settings)
{
  int result = 0;
  
  if (impl_.get_ptr ())
  {
    result = impl_->send_modifieds (prefix, settings);
  }

  return result;
}

std::string
madara::knowledge::Knowledge_Base::debug_modifieds (void) const
{
  std::string result = "";

  if (context_)
  {
    result = context_->debug_modifieds ();
  }
  else if (impl_.get_ptr ())
  {
    result = impl_->debug_modifieds ();
  }

  return result;
}

/**
  * Returns the unique host and ephemeral binding for this Knowlede Base
  * @return             host:port identifier for this knowledge base
  **/
std::string
madara::knowledge::Knowledge_Base::get_id (void)
{
  std::string result;

  if (impl_.get_ptr ())
  {
    result = impl_->get_id ();
  }

  return result;
}

/**
  * Fills a vector with Knowledge Records that begin with a common subject
  * and have a finite range of integer values.
  * @param   subject     The common subject of the variable names. For
  *                      instance, if we are looking for a range of vars
  *                      like "var0", "var1", "var2", then the common
  *                      subject would be "var".
  * @param   start       An inclusive start index
  * @param   end         An inclusive end index
  * @param   target      The vector that will be filled with
  *                      Knowledge Record instances within the subject
  *                      range.
  * @return              entries in the resulting vector
  **/
size_t
madara::knowledge::Knowledge_Base::to_vector (
                              const std::string & subject,
                              unsigned int start,
                              unsigned int end,
                              std::vector <Knowledge_Record> & target)
{
  size_t result;

  if (context_)
  {
    result = context_->to_vector (subject, start, end, target);
  }
  else if (impl_.get_ptr ())
  {
    result = impl_->to_vector (subject, start, end, target);
  }

  return result;
}

void madara::knowledge::Knowledge_Base::facade_for (
  Thread_Safe_Context & context)
{
  context_ = &context;
  impl_ = 0;
}

size_t
madara::knowledge::Knowledge_Base::to_map (
  const std::string & expression,
  std::map <std::string, Knowledge_Record> & target)
{
  size_t result;

  if (context_)
  {
    result = context_->to_map (expression, target);
  }
  else if (impl_.get_ptr ())
  {
    result = impl_->to_map (expression, target);
  }

  return result;
}

size_t
madara::knowledge::Knowledge_Base::to_map (
  const std::string & prefix,
  const std::string & delimiter,
  const std::string & suffix,
  std::vector <std::string> & next_keys,
  std::map <std::string, Knowledge_Record> & result,
  bool just_keys)
{
  size_t result_size;

  if (context_)
  {
    result_size = context_->to_map (
      prefix, delimiter, suffix, next_keys, result, just_keys);
  }
  else if (impl_.get_ptr ())
  {
    result_size = impl_->to_map (
      prefix, delimiter, suffix, next_keys, result, just_keys);
  }

  return result_size;
}
int64_t
madara::knowledge::Knowledge_Base::save_context (
  const std::string & filename) const
{
  int64_t result = 0;
  
  if (context_)
  {
    result = context_->save_context (filename);
  }
  else if (impl_.get_ptr ())
  {
    result = impl_->save_context (filename);
  }

  return result;
}


int64_t
madara::knowledge::Knowledge_Base::save_as_karl (
  const std::string & filename) const
{
  int64_t result = 0;

  if (context_)
  {
    result = context_->save_as_karl (filename);
  }
  else if (impl_.get_ptr ())
  {
    result = impl_->save_as_karl (filename);
  }

  return result;
}


int64_t
madara::knowledge::Knowledge_Base::save_checkpoint (
  const std::string & filename,
  bool reset_modifieds)
{
  int64_t result = 0;
  
  if (context_)
  {
    result = context_->save_checkpoint (filename);
  }
  else if (impl_.get_ptr ())
  {
    result = impl_->save_checkpoint (filename, reset_modifieds);
  }

  return result;
}

int64_t
madara::knowledge::Knowledge_Base::load_context (
  const std::string & filename,
  bool use_id,
  const Knowledge_Update_Settings & settings)
{
  int64_t result = 0;

  if (context_)
  {
    result = impl_->load_context (filename, use_id, settings);
  }
  else if (impl_.get_ptr ())
  {
    result = impl_->load_context (filename, use_id, settings);
  }

  return result;
}

void
madara::knowledge::Knowledge_Base::wait_for_change (void)
{
  if (context_)
  {
    context_->wait_for_change ();
  }
  else if (impl_.get_ptr ())
  {
    impl_->wait_for_change ();
  }
}

std::string
madara::knowledge::Knowledge_Base::setup_unique_hostport (
  const std::string & host)
{
  std::string result;
  
  if (impl_.get_ptr ())
  {
    result = impl_->setup_unique_hostport (host);
  }

  return result;
}