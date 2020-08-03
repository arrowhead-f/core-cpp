#include <chrono>
#include <cassert>
#include <iostream>
#include "jwt/jwt.hpp"

int main() {
	using namespace jwt::params;

	auto key = "secret"; //Secret to use for the algorithm
	//Create JWT object
	jwt::jwt_object obj{ algorithm("HS256"), payload({{"user", "admin"},{"name", "John Doe"}}), secret(key) };

	//Using add_claim to add not string claims
	obj.add_claim("iss", "marton.moro")
		.add_claim("sub", "test")
		.add_claim("id", "a-b-c-d-e-f-1-2-3")
		.add_claim("iat", 1513862371)
		.add_claim("exp", std::chrono::system_clock::now() + std::chrono::seconds{ 10 })
		;

	//Use `has_claim` to check if the claim exists or not
	assert(obj.has_claim("iss"));
	assert(obj.has_claim("exp"));
    
	//Use `has_claim_with_value` to check if the claim exists
	//with a specific value or not.
	assert(obj.payload().has_claim_with_value("id", "a-b-c-d-e-f-1-2-3"));
	assert(obj.payload().has_claim_with_value("iat", 1513862371));

	//Remove a claim using `remove_claim` API.
	//Most APIs have an overload which takes enum class type as well
	//It can be used interchangeably with strings.
	obj.remove_claim(jwt::registered_claims::expiration);
	assert(not obj.has_claim("exp"));
    obj.remove_claim("iat");
	assert(not obj.has_claim("iat"));

	bool ret = obj.payload().add_claim("sub", "new test", false/*overwrite*/);
	assert(not ret);

	//Overwrite an existing claim
	ret = obj.payload().add_claim("sub", "new test", true/*overwrite*/);
	assert(ret);

	assert(obj.payload().has_claim_with_value("sub", "new test"));

	//Get the encoded string/assertion
	auto enc_str = obj.signature();
	std::cout << enc_str << std::endl;

	//Decode
	auto dec_obj = jwt::decode(enc_str, algorithms({ "HS256" }), secret(key));
	std::cout << dec_obj.header() << std::endl;
	std::cout << dec_obj.payload() << std::endl;

	return 0;
}