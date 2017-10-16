//extra char return that wiley said


/*
 * This inclusion should be put at the beginning.  It will include <Python.h>.
 */
#include <boost/python.hpp>
#include <cstdint>
#include <string>
#include <vector>
#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>




/*
 * Create a C++ class to represent json messages
 */
class Json_Message {
    private:
        std::int timestamp;
        std::string header;
        std::float sensor_values[];
        
        // The only property: the name of the animal.
        //std::string m_name;
    public:
        // Constructor.  Note no default constructor is defined.
        Json_Message(std::int in_ts, std::string const & in_header, std::float in_sensor[]): timestamp(in_ts), header(in_header), sensor_values(in_sensor) {}
        // Copy constructor.
        //Animal(Animal const & in_other): m_name(in_other.m_name) {}
        // Copy assignment.
        
        //WHAT DID THIS DO?
        /*Animal & operator=(Animal const & in_other) {
            this->m_name = in_other.m_name;
            return *this;
        }*/
    
        // GET AND SET FUNCTIONS
        uintptr_t 
        get_address() const {
            return reinterpret_cast<uintptr_t>(this);
        }
    
        // Getter of the name property.
        std::string get_header() const {
            return this->header;
        }
        std::int get_timestamp() const {
            return this->timestamp;
        }
        std::float[] get_sensor_values() const {
            return this->sensor_values
        }
        
        // Setter of the name property.
        /*void set_name(std::string const & in_name) {
            this->m_name = in_name;
        }*/
};

/*
 * This is a macro Boost.Python provides to signify a Python extension module.
 */
BOOST_PYTHON_MODULE(Json_Message) {
    // An established convention for using boost.python.
    using namespace boost::python;

    // Expose the function hello().
    def("hello", hello);

    // Expose the class Animal.
    class_<Json_Message>("Json_Message",
        init<std::string const &>())
        .def("get_address", &Json_Message::get_address)
        .add_property("name", &Json_Message::get_name, &Animal::set_name)
    ;
}