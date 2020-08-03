#include <chrono>
#include <cassert>
#include <iostream>
#include "jwt/jwt.hpp"

int main() {
	using namespace jwt::params;

	auto key = "secret"; //Secret to use for the algorithm
	//Create JWT object using inizializer list of pair<string_view, string_view>
	jwt::jwt_object obj_pair{ algorithm("HS256"), payload({{"user", "admin"},{"name", "John Doe"}}), secret(key) };

	//Using add_claim to add not string claims
	obj_pair.add_claim("iss", "marton.moro")
		.add_claim("sub", "test")
		.add_claim("id", "a-b-c-d-e-f-1-2-3")
		.add_claim("iat", 1513862371)
		.add_claim("exp", std::chrono::system_clock::now() + std::chrono::seconds{ 10 })
		;

	//Create JWT object using  map<string, string>
	std::map<std::string, std::string> m;
	m["iss"] = "some-guy";
	m["sub"] = "something";
	m["X-pld"] = "data1";

	jwt::jwt_object obj_map{ payload(std::move(m)), secret(key), algorithm("HS256") };


	//Use `has_claim` to check if the claim exists or not
	assert(obj_pair.has_claim("iss"));
	assert(obj_pair.has_claim("exp"));
    
	//Use `has_claim_with_value` to check if the claim exists
	//with a specific value or not.
	assert(obj_pair.payload().has_claim_with_value("id", "a-b-c-d-e-f-1-2-3"));
	assert(obj_pair.payload().has_claim_with_value("iat", 1513862371));

	//Remove a claim using `remove_claim` API.
	//Most APIs have an overload which takes enum class type as well
	//It can be used interchangeably with strings.
	obj_pair.remove_claim(jwt::registered_claims::expiration);
	assert(not obj_pair.has_claim("exp"));
	obj_pair.remove_claim("iat");
	assert(not obj_pair.has_claim("iat"));

	bool ret = obj_pair.payload().add_claim("sub", "new test", false/*overwrite*/);
	assert(not ret);

	//Overwrite an existing claim
	ret = obj_pair.payload().add_claim("sub", "new test", true/*overwrite*/);
	assert(ret);

	assert(obj_pair.payload().has_claim_with_value("sub", "new test"));

	//Get the encoded string/assertion
	auto enc_str_pair = obj_pair.signature();
	std::cout << enc_str_pair << std::endl;

	auto enc_str_map = obj_map.signature();
	std::cout << enc_str_map << std::endl;


	//Decode
	auto dec_obj_pair = jwt::decode(enc_str_pair, algorithms({ "HS256" }), secret(key));
	std::cout << dec_obj_pair.header() << std::endl;
	std::cout << dec_obj_pair.payload() << std::endl;

	auto dec_obj_map = jwt::decode(enc_str_map, algorithms({ "HS256" }), secret(key));
	std::cout << dec_obj_map.header() << std::endl;
	std::cout << dec_obj_map.payload() << std::endl;

	return 0;
}