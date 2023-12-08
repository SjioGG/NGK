#include <iostream>
#include <restinio/all.hpp>
#include <json_dto/pub.hpp>

// This project was build on and then later removed alot of the when working on part 3, as i realized i had to rebuild alot of the functionality.
// Therefore the code may not compile, but it should be easy to see what i was trying to do.

struct place_t
{
	place_t() = default;

	place_t(
		std::string placeName,
		double latitude,
		double longitude)
		: m_placeName{std::move(placeName)},
		  m_latitude{latitude},
		  m_longitude{longitude}
	{
	}

	template <typename JSON_IO>
	void json_io(JSON_IO &io)
	{
		io &json_dto::mandatory("placeName", m_placeName) & json_dto::mandatory("latitude", m_latitude) & json_dto::mandatory("longitude", m_longitude);
	}

	std::string m_placeName;
	double m_latitude;
	double m_longitude;
};

struct weather_t
{
	weather_t() = default;

	weather_t(
		int id,
		std::string date,
		std::string time,
		place_t place,
		double temperature,
		int humidity)
		: m_id{id},
		  m_date{std::move(date)},
		  m_time{std::move(time)},
		  m_place{std::move(place)},
		  m_temperature{temperature},
		  m_humidity{humidity}
	{
	}

	template <typename JSON_IO>
	void json_io(JSON_IO &io)
	{
		io &json_dto::mandatory("id", m_id) & json_dto::mandatory("date", m_date) & json_dto::mandatory("time", m_time) & json_dto::mandatory("place", m_place) & json_dto::mandatory("temperature", m_temperature) & json_dto::mandatory("humidity", m_humidity);
	}

	int m_id;
	std::string m_date;
	std::string m_time;
	place_t m_place;
	double m_temperature;
	int m_humidity;
};

using weather_data_t = std::vector<weather_t>;


class weather_handler_t
{
public:
	explicit weather_handler_t(weather_data_t &weather)
		: m_weather(weather)
	{
	}

	weather_handler_t(const weather_handler_t &) = delete;
	weather_handler_t(weather_handler_t &&) = delete;

	// Helper print function to optimmize the code:
	std::string format_weather_data(const weather_t &w) const{
		return "ID: " + std::to_string(w.m_id) + "\n"
			"Tidspunkt (Dato og klokkeslæt) \n"
			"  Dato: " + w.m_date + "\n"
			"  Klokkeslæt: " + w.m_time + "\n"
			"Sted: \n"
			"  Navn: " + w.m_place.m_placeName + "\n"
			"  Lat: " + std::to_string(w.m_place.m_latitude) + "\n"
			"  Lon: " + std::to_string(w.m_place.m_longitude) + "\n"
			"Temperatur: " + std::to_string(w.m_temperature) + "\n"
			"Luftfugtighed: " + std::to_string(w.m_humidity) + "\n\n";
	}

	auto on_weather_list(const restinio::request_handle_t &req, rr::route_params_t) const {
		auto resp = init_resp(req->create_response());

		resp.set_body("Weather data (count: " + std::to_string(m_weather.size()) + ")\n");

		for (std::size_t i = 0; i < m_weather.size(); ++i) {
			resp.append_body(std::to_string(i + 1) + ". ");
			const auto &w = m_weather[i];
			resp.append_body(format_weather_data(w));
		}

		return resp.done();
	}

	auto on_weather_get(const restinio::request_handle_t &req, rr::route_params_t params) {
		const auto weather_id = restinio::cast_to<int>(params["weather_id"]);

		auto resp = init_resp(req->create_response());

		if (weather_id >= 0 && static_cast<std::size_t>(weather_id) < m_weather.size()) {
			const auto &w = m_weather[weather_id];
			resp.set_body(format_weather_data(w) + "This output is from the GET FUNCTION");
		} else {
			resp.set_body("No weather data with ID " + std::to_string(weather_id) + "\n");
		}

		return resp.done();
	}

	auto on_weather_get_limit(const restinio::request_handle_t &req, rr::route_params_t params) {
		auto resp = init_resp(req->create_response());

		int limit = restinio::cast_to<int>(params["limit"]);
		if (limit < 0) {
			resp.set_body("Limit must be non-negative.\n");
			return resp.done();
		}

		std::size_t start_index = 0;
		if (m_weather.size() > static_cast<std::size_t>(limit)) {
			start_index = m_weather.size() - limit;
		}

		resp.set_body("Latest " + std::to_string(limit) + " weather data:\n");

		for (std::size_t i = start_index; i < m_weather.size(); ++i) {
			resp.append_body(std::to_string(i + 1) + ". ");
			const auto &w = m_weather[i];
			resp.append_body(format_weather_data(w));
		}

		return resp.done();
	}

	auto on_date_get(const restinio::request_handle_t &req, rr::route_params_t params) {
		auto resp = init_resp(req->create_response());
		try {
			auto date = restinio::utils::unescape_percent_encoding(params["date"]);

			resp.set_body("Weather data on " + date + ":\n");

			for (std::size_t i = 0; i < m_weather.size(); ++i) {
				const auto &w = m_weather[i];
				if (date == w.m_date) { // assuming m_date is a string
					resp.append_body(std::to_string(i + 1) + ". ");
					resp.append_body(format_weather_data(w));
				}
			}
		} catch (const std::exception &) {
			mark_as_bad_request(resp);
		}

		return resp.done();
	}

	auto on_weather_new(
		const restinio::request_handle_t &req, rr::route_params_t)
	{
		auto resp = init_resp(req->create_response());

		try
		{
			m_weather.emplace_back(
				json_dto::from_json<weather_t>(req->body()));
		}
		catch (const std::exception &)
		{
			mark_as_bad_request(resp);
		}

		return resp.done();
	}

	auto on_weather_update(
		const restinio::request_handle_t &req, rr::route_params_t params)
	{
		const auto weather_id = restinio::cast_to<int>(params["weather_id"]);

		auto resp = init_resp(req->create_response());

		try
		{
			auto updated_weather = json_dto::from_json<weather_t>(req->body());

			if (weather_id >= 0 && static_cast<std::size_t>(weather_id) < m_weather.size())
			{
				m_weather[weather_id] = updated_weather;
			}
			else
			{
				mark_as_bad_request(resp);
				resp.set_body("No weather data with ID " + std::to_string(weather_id) + "\n");
			}
		}
		catch (const std::exception &)
		{
			mark_as_bad_request(resp);
		}

		return resp.done();
	}

	// Ignore
	auto on_weather_delete(
		const restinio::request_handle_t &req, rr::route_params_t params)
	{
		const auto weather_id = restinio::cast_to<int>(params["weather_id"]);

		auto resp = init_resp(req->create_response());

		if (weather_id >= 0 && static_cast<std::size_t>(weather_id) < m_weather.size())
		{
			const auto &w = m_weather[weather_id];
			resp.set_body(
				"ID: " + std::to_string(w.m_id) + "\n"
												  "Dato: " +
				w.m_date + "\n"
						   "Klokkeslæt: " +
				w.m_time + "\n"
						   "Sted: \n"
						   "  Navn: " +
				w.m_place.m_placeName + "\n"
										"  Lat: " +
				std::to_string(w.m_place.m_latitude) + "\n"
													   "  Lon: " +
				std::to_string(w.m_place.m_longitude) + "\n"
														"Temperatur: " +
				std::to_string(w.m_temperature) + "\n"
												  "Luftfugtighed: " +
				std::to_string(w.m_humidity) + "\n");

			m_weather.erase(m_weather.begin() + weather_id);
		}
		else
		{
			resp.set_body(
				"No weather data with ID " + std::to_string(weather_id) + "\n");
		}

		return resp.done();
	}

private:
	weather_data_t &m_weather;

	template <typename RESP>
	static void
	mark_as_bad_request(RESP &resp)
	{
		resp.header().status_line(restinio::status_bad_request());
	}
};

auto server_handler(weather_data_t &weather_data)
{
	auto router = std::make_unique<router_t>();
	auto handler = std::make_shared<weather_handler_t>(std::ref(weather_data));

	auto by = [&](auto method)
	{
		using namespace std::placeholders;
		return std::bind(method, handler, _1, _2);
	};

	auto method_not_allowed = [](const auto &req, auto)
	{
		return req->create_response(restinio::status_method_not_allowed())
			.connection_close()
			.done();
	};

	// Handlers for '/' path.
	router->http_get("/", by(&weather_handler_t::on_weather_list));
	router->http_post("/", by(&weather_handler_t::on_weather_new));

	// Disable all other methods for '/weather'.
	router->add_handler(
		restinio::router::none_of_methods(
			restinio::http_method_get(),
			restinio::http_method_post()),
		"/", method_not_allowed);

	// Handler for '/:weather_id' path
	router->http_get(
		R"(/:weather_id(\d+))",
		by(&weather_handler_t::on_weather_get));
	router->http_get(
		R"(/limit/:limit(\d+))",
		by(&weather_handler_t::on_weather_get_limit));
	router->http_put(
		R"(/:weather_id(\d+))",
		by(&weather_handler_t::on_weather_update));
	router->http_delete(
		R"(/:weather_id(\d+))",
		by(&weather_handler_t::on_weather_delete));

	 // Disable all other methods for '/weather/:weather_id'.
	router->add_handler(
	 		restinio::router::none_of_methods(
	 		restinio::http_method_get(),
	 		restinio::http_method_post(),
	 		restinio::http_method_delete()),
	 	R"(/:weather_id(\d+))", method_not_allowed);

	// Handler for '/location/:location' path.
	router->http_get("/date/:date", by(&weather_handler_t::on_date_get));
	router->http_get("/chat", by(&weather_handler_t::on_live_update));

	// Disable all other methods for '/location/:location'.
	router->add_handler(
		restinio::router::none_of_methods(restinio::http_method_get()),
		"/date/:date", method_not_allowed);

	return router;
}

int main()
{
	using namespace std::chrono;

	try
	{
		using traits_t =
			restinio::traits_t<
				restinio::asio_timer_manager_t,
				restinio::single_threaded_ostream_logger_t,
				router_t>;

		weather_data_t weather_data{
			{1, "20211105", "12:15", {"Aarhus N", 13.692, 19.438}, 13.1, 70}};
		// SAMPLE: {2, "20211206", "08:15", {"Tokyo", 35.6895, 139.6917}, 22.3, 90}};

		restinio::run(
			restinio::on_this_thread<traits_t>()
				.address("localhost")
				.request_handler(server_handler(weather_data))
				.read_next_http_message_timelimit(10s)
				.write_http_response_timelimit(1s)
				.handle_request_timeout(1s));
	}
	catch (const std::exception &ex)
	{
		std::cerr << "Error: " << ex.what() << std::endl;
		return 1;
	}

	return 0;
}
