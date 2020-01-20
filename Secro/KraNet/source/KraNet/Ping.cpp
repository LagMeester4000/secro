#include "KraNet/Ping.h"

kra::net::Ping::Ping()
{
	Pings.PushBack(20.0);
}

void kra::net::Ping::Add(double NewPing)
{
	Pings.PushBack(NewPing);
}

double kra::net::Ping::Get() const
{
	double TotalPing = 0.0;
	for (size_t I = Pings.Bottom(); I < Pings.Top(); ++I)
	{
		TotalPing += Pings[I];
	}
	return TotalPing / (double)Pings.Size();
}
