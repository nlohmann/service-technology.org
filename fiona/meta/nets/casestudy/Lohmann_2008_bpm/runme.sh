#!/bin/sh

cd bpm2008

for example in \
	auction_service car_return \
	credit_preparation \
	customer_service \
	internal_order \
	loan_approval \
	online_shop \
	order_process \
	purchase_order \
	register_request \
	supply_order
do
  echo "=============================================================================="
  echo "executing  rachel -a $example.sa -o $example.og -v"
  echo "=============================================================================="
  rachel -a $example.sa -o $example.og -v
  echo ""
done
